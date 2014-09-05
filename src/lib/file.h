#ifndef FILE_H
#define FILE_H

#include <QtCore/QIODevice>
#include <QtCore/QFileDevice>
#include <QtCore/QFuture>
#include <QtCore/QUrl>

class FilePrivate;
class QIO_EXPORT File : public QIODevice
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(File)
    Q_DISABLE_COPY(File)

    Q_PROPERTY(QUrl url READ url WRITE setUrl NOTIFY urlChanged)

public:
    enum State { Closed, Opening, Opened };

    explicit File(QObject *parent = 0);
    ~File();

    bool open(OpenMode mode);
    QFuture<bool> asyncOpen(OpenMode mode);
    void close();

    qint64 size() const;
    bool seek(qint64 pos);

    qint64 bytesAvailable() const;
    qint64 bytesToWrite() const;

    bool waitForBytesWritten(int msecs = -1);
    bool waitForReadyRead(int msecs = -1);

    QUrl url() const;

    State state() const;

    QFileDevice::FileError error() const;

public slots:
    void setUrl(const QUrl &url);

signals:
    void opened(bool ok);
    void urlChanged(QUrl arg);

protected:
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 maxlen);

    FilePrivate *d_ptr;

    friend class AbstractFileEngine;

private slots:
    void onOpenFinished();
};

#endif // FILE_H
