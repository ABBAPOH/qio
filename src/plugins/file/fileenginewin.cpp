#include "fileenginewin.h"

#include <QIO/RunExtensions>

typedef QFuture<HANDLE> OpenFuture;
typedef QFutureInterface<HANDLE> OpenFutureInterface;

static QString errorMessage(DWORD error)
{
    void *lpMsgBuf;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER
                  | FORMAT_MESSAGE_FROM_SYSTEM
                  | FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL,
                  error,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (wchar_t *) &lpMsgBuf,
                  0, NULL );

    auto res = QString::fromWCharArray((wchar_t*)lpMsgBuf);
    LocalFree(lpMsgBuf);
    return res;
}

FileEngineWin::FileEngineWin() :
    openWatcher(new QFutureWatcher<HANDLE>(this)),
    m_FileHandle(INVALID_HANDLE_VALUE),
    reading(false),
    activeOverlapped(new MyOverlapped(this)),
    inactiveOverlapped(new MyOverlapped(this)),
    pos(0)
{
    connect(openWatcher, &QFutureWatcherBase::finished, this, &FileEngineWin::onOpenFinished);
}

void FileEngineWin::open(QIODevice::OpenMode mode)
{
    typedef void (*func)(OpenFutureInterface &future, const QString path, QIODevice::OpenMode mode);
    func f = [](OpenFutureInterface &future, const QString path, QIODevice::OpenMode mode) {
        HANDLE fileHandle = CreateFile(reinterpret_cast<const wchar_t *>(path.data()), // file to open
                                       GENERIC_READ,           // open for reading
                                       FILE_SHARE_READ | FILE_SHARE_DELETE,        // share for reading
                                       NULL,                   // default security
                                       OPEN_EXISTING,          // existing file only
                                       FILE_FLAG_OVERLAPPED,   // overlapped operation
                                       NULL);                  // no attr. template

        if (fileHandle == INVALID_HANDLE_VALUE) {
            qWarning() << "failed to open file" << GetLastError();
        }

        future.reportResult(fileHandle);
    };
    const QString path = url().toLocalFile();
    openWatcher->setFuture(QtConcurrent::run(f, path, mode));
}

bool FileEngineWin::waitForOpened(int msecs)
{
    Q_UNUSED(msecs);
    auto future = openWatcher->future();
    if (future == OpenFuture())
        return false;
    future.waitForFinished();
    onOpenFinished();
    return true;
}

void FileEngineWin::close()
{
    if (m_FileHandle == INVALID_HANDLE_VALUE)
        return;

    if (reading) {
        activeOverlapped->canceled = true;
        CancelIo(m_FileHandle); // seems has no effect
        WaitForSingleObjectEx(activeOverlapped->hEvent, -1, true);
    }

    CloseHandle(m_FileHandle);
    m_FileHandle = INVALID_HANDLE_VALUE;
}

bool FileEngineWin::seek(qint64 pos)
{
    this->pos = pos;
    // TODO: cancel current job
    return true;
}

qint64 FileEngineWin::size() const
{
    if (!m_FileHandle)
        return 0;
    quint32 high, low;
    low = GetFileSize(m_FileHandle, (LPDWORD)&high);
    return (quint64(high) << 32) + low;
}

void FileEngineWin::read(qint64 maxlen)
{
    Q_ASSERT(maxlen > 0);
//    qDebug() << "FileEngineWin::read" << "maxlen =" << maxlen;
    if (reading)
        return;

    if (!m_FileHandle)
        return;

    readBuffer.resize(maxlen);
    const qint64 offset = pos;
    activeOverlapped->Offset = offset;
    activeOverlapped->canceled = false;
//    qDebug() << "FileEngineWin::read" << "active" << QString::number(qintptr(activeOverlapped.data()), 16);
//    qDebug() << "FileEngineWin::read" << "inactive" << QString::number(qintptr(inactiveOverlapped.data()), 16);

    bool ok = ReadFileEx(m_FileHandle,
                         readBuffer.data(),
                         maxlen,
//                             &dwBytesRead,
                         activeOverlapped.data(),
                         &readCallback);
    pos += maxlen;
    if (!ok) {
        qWarning() << "ReadFileEx failed" << errorMessage(GetLastError());
//        setError(QFileDevice::ReadError, errorMessage(GetLastError()) );
    }
    reading = ok;
}

bool FileEngineWin::waitForBytesWritten(int msecs)
{
    return false;
}

bool FileEngineWin::waitForReadyRead(int msecs)
{
    DWORD result = WaitForSingleObjectEx(activeOverlapped->hEvent, msecs, true);
//    qDebug() << "waitForReadyRead finished" << errorMessage(dwWaitOvpOprn);
    switch (result) {
    case WAIT_FAILED:
        return false;
    case WAIT_OBJECT_0:
        return true;
    case WAIT_TIMEOUT:
        return false;
    }

    return true;
}

void FileEngineWin::readCallback(DWORD errorCode, DWORD numberOfBytesTransfered, LPOVERLAPPED overlapped)
{
//    qDebug() << "FileEngineWin::readCallback" << "errorCode =" << errorCode
//             << "numberOfBytesTransfered = " << numberOfBytesTransfered;
//    qDebug() << "FileEngineWin::readCallback" << "overlapped" << QString::number(qintptr(overlapped), 16);

    MyOverlapped *myOverlapped = static_cast<MyOverlapped *>(overlapped);
    FileEngineWin *engine = myOverlapped->engine;
    engine->reading = false;
    if (myOverlapped->canceled || errorCode == ERROR_OPERATION_ABORTED) {
        qDebug() << "FileEngineWin::readCallback" << "operation aborted";
        return;
    }
    if (errorCode != NOERROR) {
        qDebug() << "FileEngineWin::readCallback" << "errorCode =" << errorCode
                 << "numberOfBytesTransfered = " << numberOfBytesTransfered;
            return;
    }
    engine->activeOverlapped.swap(engine->inactiveOverlapped);
    if (numberOfBytesTransfered != engine->readBuffer.size())
        numberOfBytesTransfered = 0;
    engine->readFinished(engine->readBuffer.data(), numberOfBytesTransfered);
}

void FileEngineWin::onOpenFinished()
{
    auto future = openWatcher->future();
    if (future == OpenFuture())
        return;
    m_FileHandle = future.result();

    if (m_FileHandle == INVALID_HANDLE_VALUE)
        openFinished(false);
    else
        openFinished(true);

    openWatcher->setFuture(OpenFuture());
}
