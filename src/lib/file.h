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
    Q_PROPERTY(int bufferSize READ bufferSize WRITE setBufferSize NOTIFY bufferSizeChanged)

public:
    enum State { Closed, Opening, Opened };

    explicit File(QObject *parent = 0);
    explicit File(const QUrl &url, QObject *parent = 0);
    ~File();

    bool open(OpenMode mode) Q_DECL_OVERRIDE;
    void asyncOpen(OpenMode mode);
    bool waitForOpened(int msecs = -1);
    void close() Q_DECL_OVERRIDE;

    qint64 size() const Q_DECL_OVERRIDE;
    bool seek(qint64 pos) Q_DECL_OVERRIDE;
    bool atEnd() const Q_DECL_OVERRIDE;

    qint64 bytesAvailable() const Q_DECL_OVERRIDE;
    qint64 bytesToWrite() const Q_DECL_OVERRIDE;

    bool waitForBytesWritten(int msecs = -1) Q_DECL_OVERRIDE;
    bool waitForReadyRead(int msecs = -1) Q_DECL_OVERRIDE;

    QUrl url() const;

    State state() const;

    int bufferSize() const;

    QFileDevice::FileError error() const;

public slots:
    void setUrl(const QUrl &url);

    void setBufferSize(int size);

signals:
    void opened(bool ok);
    void urlChanged(QUrl arg);

    void bufferSizeChanged(int arg);

protected:
    qint64 readData(char *data, qint64 maxlen) Q_DECL_OVERRIDE;
    qint64 writeData(const char *data, qint64 maxlen) Q_DECL_OVERRIDE;

    FilePrivate *d_ptr;

    friend class AbstractFileEngine;
};

#endif // FILE_H
