#ifndef ABSTRACTFILEENGINE_H
#define ABSTRACTFILEENGINE_H

#include "file.h"

#include <QtCore/QFuture>
#include <QtCore/QObject>

class AbstractFileEnginePrivate;
class QIO_EXPORT AbstractFileEngine : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AbstractFileEngine)
    Q_DISABLE_COPY(AbstractFileEngine)
public:
    explicit AbstractFileEngine(QObject *parent = Q_NULLPTR);
    virtual ~AbstractFileEngine();

    QUrl url() const;
    virtual void setUrl(const QUrl &url);

    virtual void open(QIODevice::OpenMode mode) = 0;
    virtual bool waitForOpened(int msecs = -1) = 0;
    virtual void close() = 0;

    virtual bool seek(qint64 pos) = 0;
    virtual qint64 size() const = 0;

    virtual void read(qint64 maxlen) = 0;
    virtual void write(const QByteArray &data) = 0;

    virtual bool waitForBytesWritten(int msecs = -1) = 0;
    virtual bool waitForReadyRead(int msecs = -1) = 0;

    static AbstractFileEngine *emptyEngine();

protected:
    void openFinished(bool ok);
    void readFinished(const char *data, qint64 length);
    void writeFinished(qint64 length);

protected:
    AbstractFileEnginePrivate *d_ptr;

    friend class File;
};

#endif // ABSTRACTFILEENGINE_H
