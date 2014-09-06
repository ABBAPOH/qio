#ifndef FILEENGINEWIN_H
#define FILEENGINEWIN_H

#include "fileengine.h"

#include <Windows.h>

static const int BUFFER_SIZE =4* 1024;

class FileEngineWin : public FileEngine
{
public:
    FileEngineWin();

    void open(QIODevice::OpenMode mode) Q_DECL_OVERRIDE;
    bool waitForOpened(int msecs) Q_DECL_OVERRIDE;
    void close() Q_DECL_OVERRIDE;

    bool seek(qint64 pos) Q_DECL_OVERRIDE;
    qint64 size() const Q_DECL_OVERRIDE;
    void read(qint64 maxlen) Q_DECL_OVERRIDE;

    bool waitForBytesWritten(int msecs) Q_DECL_OVERRIDE;
    bool waitForReadyRead(int msecs) Q_DECL_OVERRIDE;

private:
    static void readCallback(DWORD errorCode, DWORD numberOfBytesTransfered, LPOVERLAPPED overlapped);

private:
    HANDLE m_FileHandle;
    bool reading;

    struct MyOverlapped : public OVERLAPPED
    {
        MyOverlapped(FileEngineWin *engine)
        {
//            Q_ASSERT((void *)&overlapped == (void *)this);
            memset(this, 0, sizeof(MyOverlapped));
            hEvent = CreateEvent(0,TRUE,FALSE,0);
            this->engine = engine;
        }

        FileEngineWin *engine;
        char buffer[BUFFER_SIZE];
    };

    MyOverlapped overlapped;
    qint64 pos;
};

#endif // FILEENGINEWIN_H
