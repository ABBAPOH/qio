#ifndef FILEENGINEFALLBACK_H
#define FILEENGINEFALLBACK_H

#include <QIO/AbstractFileEngine>

#include <QtCore/QFile>
#include <QtCore/QMutex>

class FileEngineFallback : public AbstractFileEngine
{
    Q_OBJECT
public:
    explicit FileEngineFallback(QObject *parent = Q_NULLPTR);

    void setUrl(const QUrl &url) Q_DECL_OVERRIDE;

    void open(QIODevice::OpenMode mode) Q_DECL_OVERRIDE;
    bool waitForOpened(int msecs) Q_DECL_OVERRIDE;
    void close() Q_DECL_OVERRIDE;

    bool seek(qint64 pos) Q_DECL_OVERRIDE;
    qint64 size() const Q_DECL_OVERRIDE;

    void read(qint64 maxlen) Q_DECL_OVERRIDE;

    bool waitForBytesWritten(int msecs = -1) Q_DECL_OVERRIDE;
    bool waitForReadyRead(int msecs = -1) Q_DECL_OVERRIDE;

private slots:
    void onOpenFinished();
    void onReadFinished();

private:

    struct ThreadData
    {
        ThreadData() : file(Q_NULLPTR) {}

        QFile *file;
        QByteArray readBuffer;
        mutable QMutex mutex;
    } data;

    QFuture<bool> openFuture;
};

#endif // FILEENGINEFALLBACK_H
