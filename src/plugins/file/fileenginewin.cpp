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
    func f = [](OpenFutureInterface &future, const QString path, QIODevice::OpenMode openMode) {
        const DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;

        DWORD accessRights = 0;
        if (openMode & QIODevice::ReadOnly)
            accessRights |= GENERIC_READ;
        if (openMode & QIODevice::WriteOnly)
            accessRights |= GENERIC_WRITE;

        const DWORD creationDisposition = (openMode & QIODevice::WriteOnly)
                ? OPEN_ALWAYS
                : OPEN_EXISTING;
        HANDLE fileHandle = CreateFile(reinterpret_cast<const wchar_t *>(path.data()),
                                       accessRights,
                                       shareMode,
                                       NULL,
                                       creationDisposition,
                                       FILE_FLAG_OVERLAPPED,
                                       NULL);

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

void FileEngineWin::write(const QByteArray &data)
{
    Q_ASSERT(m_FileHandle != INVALID_HANDLE_VALUE);
    if (m_FileHandle == INVALID_HANDLE_VALUE)
        return;

    qDebug() << "FileEngineWin::write" << "length =" << data.size();

    writeBuffer = data;
    activeOverlapped->Offset = pos;
    activeOverlapped->canceled = false;

    bool ok = WriteFileEx(m_FileHandle,
                          writeBuffer.data(),
                          writeBuffer.size(),
                          activeOverlapped.data(),
                          &writeCallback);
    if (!ok)
        qWarning() << "WriteFileEx failed" << errorMessage(GetLastError());
}

static bool waitForEvent(HANDLE event, int msecs)
{
    DWORD result = WaitForSingleObjectEx(event, msecs, true);
    switch (result) {
    case WAIT_OBJECT_0:
        return true;
    case WAIT_ABANDONED:
        qWarning() << "waitForEvent" << "WAIT_ABANDONED";
        return false;
    case WAIT_IO_COMPLETION:
        qWarning() << "waitForEvent" << "WAIT_IO_COMPLETION";
        return false;
    case WAIT_FAILED:
        qWarning() << "waitForEvent" << "WAIT_FAILED :"
                   << errorMessage(GetLastError());
        return false;
    case WAIT_TIMEOUT:
        return false;
    default:
        break;
    }
    Q_UNREACHABLE();

    return false;
}

bool FileEngineWin::waitForBytesWritten(int msecs)
{
    qDebug() << "waitForBytesWritten";
    return waitForEvent(activeOverlapped->hEvent, msecs);
}

bool FileEngineWin::waitForReadyRead(int msecs)
{
    qDebug() << "waitForReadyRead";
    return waitForEvent(activeOverlapped->hEvent, msecs);
}

void FileEngineWin::readCallback(DWORD errorCode, DWORD numberOfBytesTransfered, LPOVERLAPPED overlapped)
{
    qDebug() << "FileEngineWin::readCallback" << "errorCode =" << errorCode
             << "numberOfBytesTransfered = " << numberOfBytesTransfered;
    qDebug() << "FileEngineWin::readCallback" << "overlapped" << QString::number(qintptr(overlapped), 16);

    MyOverlapped *myOverlapped = static_cast<MyOverlapped *>(overlapped);
    FileEngineWin *engine = myOverlapped->engine;
    engine->reading = false;
    if (myOverlapped->canceled || errorCode == ERROR_OPERATION_ABORTED) {
        myOverlapped->canceled = false;
        qDebug() << "FileEngineWin::readCallback" << "operation aborted";
        return;
    }
    if (errorCode != NOERROR) {
        qDebug() << "FileEngineWin::readCallback" << "errorCode =" << errorCode
                 << "numberOfBytesTransfered = " << numberOfBytesTransfered;
            return;
    }
    engine->activeOverlapped.swap(engine->inactiveOverlapped);
    if (numberOfBytesTransfered != static_cast<uint>(engine->readBuffer.size()))
        numberOfBytesTransfered = 0;
    engine->readFinished(engine->readBuffer.data(), numberOfBytesTransfered);
    qDebug() << "readCallback finished";
}

void FileEngineWin::writeCallback(DWORD errorCode, DWORD numberOfBytesTransfered, LPOVERLAPPED overlapped)
{
    qDebug() << "FileEngineWin::writeCallback" << "errorCode =" << errorCode
             << "numberOfBytesTransfered = " << numberOfBytesTransfered;

    MyOverlapped *myOverlapped = static_cast<MyOverlapped *>(overlapped);
    FileEngineWin *engine = myOverlapped->engine;
    engine->writeBuffer.clear();
    engine->activeOverlapped.swap(engine->inactiveOverlapped);
    engine->writeFinished(numberOfBytesTransfered);
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
