#include "fileenginewin.h"

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
    m_FileHandle(INVALID_HANDLE_VALUE),
    reading(false),
    activeOverlapped(new MyOverlapped(this)),
    inactiveOverlapped(new MyOverlapped(this)),
    pos(0)
{
}

void FileEngineWin::open(QIODevice::OpenMode mode)
{
    const QString path = url().toLocalFile();
    m_FileHandle = CreateFile(reinterpret_cast<const wchar_t *>(path.data()), // file to open
                              GENERIC_READ,           // open for reading
                              FILE_SHARE_READ | FILE_SHARE_DELETE,        // share for reading
                              NULL,                   // default security
                              OPEN_EXISTING,          // existing file only
                              FILE_FLAG_OVERLAPPED,   // overlapped operation
                              NULL);                  // no attr. template

    qDebug() << "FileEngineWin::open" << qintptr(m_FileHandle), 16;
    if (m_FileHandle == INVALID_HANDLE_VALUE) {
        openFinished(false);
        return;
    }

    openFinished(true);
}

bool FileEngineWin::waitForOpened(int msecs)
{
    return true;
}

void FileEngineWin::close()
{
    if (!m_FileHandle)
        return;

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
    qDebug() << "FileEngineWin::read" << "maxlen =" << maxlen;
    if (reading)
        return;

    if (!m_FileHandle)
        return;

    readBuffer.resize(maxlen);
    const qint64 offset = pos;
    activeOverlapped->Offset = offset;
    qDebug() << "FileEngineWin::read" << "active" << QString::number(qintptr(activeOverlapped.data()), 16);
    qDebug() << "FileEngineWin::read" << "inactive" << QString::number(qintptr(inactiveOverlapped.data()), 16);

    bool ok = ReadFileEx(m_FileHandle,
                         readBuffer.data(),
                         maxlen,
//                             &dwBytesRead,
                         activeOverlapped.data(),
                         &readCallback);
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
    qDebug() << "FileEngineWin::readCallback" << "overlapped" << QString::number(qintptr(overlapped), 16);
    qDebug() << "FileEngineWin::readCallback" << "numberOfBytesTransfered = " << numberOfBytesTransfered;
    MyOverlapped *myOverlapped = static_cast<MyOverlapped *>(overlapped);
    FileEngineWin *engine = myOverlapped->engine;
    engine->reading = false;
    engine->pos += numberOfBytesTransfered;
    engine->activeOverlapped.swap(engine->inactiveOverlapped);
    engine->readFinished(engine->readBuffer.data(), numberOfBytesTransfered);
}
