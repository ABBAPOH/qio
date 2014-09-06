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
    reading(false),
    overlapped(this),
    pos(0)
{
}

void FileEngineWin::open(QIODevice::OpenMode mode)
{
    const QString path = url().toLocalFile();
    m_FileHandle = CreateFile(reinterpret_cast<const wchar_t *>(path.data()), // file to open
                              GENERIC_READ,           // open for reading
                              FILE_SHARE_READ,        // share for reading
                              NULL,                   // default security
                              OPEN_EXISTING,          // existing file only
                              FILE_FLAG_OVERLAPPED,   // overlapped operation
                              NULL);                  // no attr. template

    if (m_FileHandle == INVALID_HANDLE_VALUE) {
        openFinished(false);
        return;
    }

//    if (!asyncRead(pos())) {
//        setError(QFileDevice::OpenError, errorMessage(GetLastError()));
//        return false;
//    }
    openFinished(true);
}

bool FileEngineWin::waitForOpened(int msecs)
{
    return true;
}

void FileEngineWin::close()
{
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
    qDebug() << "FileEngineWin::read" << maxlen;
    if (reading)
        return;

//    qint64 bytes = qMin(BUFFER_SIZE, MAX_BUFFER_SIZE - m_readBuffer.size());
    qint64 bytes = maxlen;
    const qint64 offset = pos;
//    bytes = qMin(bytes, size() - offset);
//    qDebug() << "asyncRead" << "pos" << pos << "size" << size() << "bytes" << bytes;
//    if (bytes <= 0)
//        return true;
//    Q_ASSERT(offset + bytes <= size());
    overlapped.Offset = offset;

    bool ok = ReadFileEx(m_FileHandle,
                         overlapped.buffer,
                         bytes,
//                             &dwBytesRead,
                         &overlapped,
                         &readCallback);
    if (!ok) {
        qWarning() << "ReadFileEx failed" << errorMessage(GetLastError());
//        setError(QFileDevice::ReadError, errorMessage(GetLastError()) );
    }
    reading = ok;
    qDebug() << "asyncRead" << "finished" << ok;
//    return ok;
}

bool FileEngineWin::waitForBytesWritten(int msecs)
{
    return false;
}

bool FileEngineWin::waitForReadyRead(int msecs)
{
    DWORD result = WaitForSingleObjectEx(overlapped.hEvent, msecs, true);
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
    qDebug() << "readCallback" << numberOfBytesTransfered;
    MyOverlapped *myOverlapped = reinterpret_cast<MyOverlapped *>(overlapped);
    myOverlapped->engine->reading = false;
    myOverlapped->engine->pos += numberOfBytesTransfered;
    myOverlapped->engine->readFinished(myOverlapped->buffer, numberOfBytesTransfered);

}
