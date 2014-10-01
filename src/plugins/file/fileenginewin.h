#ifndef FILEENGINEWIN_H
#define FILEENGINEWIN_H

#include <QIO/AbstractFileEngine>
#include <QtCore/QFutureWatcher>
#include <Windows.h>

class FileEngineWin : public AbstractFileEngine
{
    Q_OBJECT
public:
    FileEngineWin();

    void open(QIODevice::OpenMode mode) Q_DECL_OVERRIDE;
    bool waitForOpened(int msecs) Q_DECL_OVERRIDE;
    void close() Q_DECL_OVERRIDE;

    bool seek(qint64 pos) Q_DECL_OVERRIDE;
    qint64 size() const Q_DECL_OVERRIDE;
    void read(qint64 maxlen) Q_DECL_OVERRIDE;
    void write(const QByteArray &data) Q_DECL_OVERRIDE;

    bool waitForBytesWritten(int msecs) Q_DECL_OVERRIDE;
    bool waitForReadyRead(int msecs) Q_DECL_OVERRIDE;

private:
    static void WINAPI readCallback(DWORD errorCode, DWORD numberOfBytesTransfered, LPOVERLAPPED activeOverlapped);
    static void WINAPI writeCallback(DWORD errorCode, DWORD numberOfBytesTransfered, LPOVERLAPPED overlapped);

private slots:
    void onOpenFinished();

private:
    QFutureWatcher<HANDLE> *openWatcher;
    HANDLE m_FileHandle;
    bool reading;

    struct MyOverlapped : public OVERLAPPED
    {
        MyOverlapped(FileEngineWin *engine)
        {
            Q_ASSERT(qintptr(static_cast<LPOVERLAPPED>(this)) == qintptr(this));
            memset(this, 0, sizeof(MyOverlapped));
            hEvent = CreateEvent(Q_NULLPTR, true, false, Q_NULLPTR);
            this->engine = engine;
            canceled = false;
        }
        ~MyOverlapped()
        {
            CloseHandle(hEvent);
        }

        FileEngineWin *engine;
        bool canceled;
    };

    QScopedPointer<MyOverlapped> activeOverlapped;
    QScopedPointer<MyOverlapped> inactiveOverlapped;
//    char readBuffer[BUFFER_SIZE];
    QByteArray readBuffer;
    QByteArray writeBuffer;
    qint64 pos;
};

#endif // FILEENGINEWIN_H
