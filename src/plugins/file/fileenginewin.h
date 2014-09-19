#ifndef FILEENGINEWIN_H
#define FILEENGINEWIN_H

#include <QIO/AbstractFileEngine>

#include <Windows.h>

class FileEngineWin : public AbstractFileEngine
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
    static void readCallback(DWORD errorCode, DWORD numberOfBytesTransfered, LPOVERLAPPED activeOverlapped);

private:
    HANDLE m_FileHandle;
    bool reading;

    struct MyOverlapped : public OVERLAPPED
    {
        MyOverlapped(FileEngineWin *engine)
        {
            Q_ASSERT(qintptr(static_cast<LPOVERLAPPED>(this)) == qintptr(this));
            memset(this, 0, sizeof(MyOverlapped));
            hEvent = CreateEvent(0,TRUE,FALSE,0);
            qDebug() << "MyOverlapped" << "hEvent =" << QString::number(qintptr(hEvent), 16);
            this->engine = engine;
        }
        ~MyOverlapped()
        {
            CloseHandle(hEvent);
        }

        FileEngineWin *engine;
    };

    QScopedPointer<MyOverlapped> activeOverlapped;
    QScopedPointer<MyOverlapped> inactiveOverlapped;
//    char readBuffer[BUFFER_SIZE];
    QByteArray readBuffer;
    qint64 pos;
};

#endif // FILEENGINEWIN_H
