#ifndef ABSTRACTFILEENGINE_H
#define ABSTRACTFILEENGINE_H

#include "file.h"
#include "fileinfo.h"

#include <QtCore/QFuture>

class AbstractFileEnginePrivate;
class AbstractFileEngine
{
    Q_DECLARE_PRIVATE(AbstractFileEngine)
    Q_DISABLE_COPY(AbstractFileEngine)
public:
    AbstractFileEngine();
    virtual ~AbstractFileEngine();

    QUrl url() const;
    virtual void setUrl(const QUrl &url);

    virtual QFuture<bool> open(QIODevice::OpenMode mode) = 0;
//    virtual bool waitForOpened(int msecs = -1) = 0;
    virtual void close() = 0;

    virtual bool seek(qint64 pos) = 0;

    virtual bool waitForBytesWritten(int msecs = -1) = 0;
    virtual bool waitForReadyRead(int msecs = -1) = 0;

    virtual QFuture<QString> list() = 0;
    virtual QFuture<FileInfo> entryList() = 0;
    virtual QFuture<bool> mkdir(const QString &fileName) = 0;
    // TODO: separate rmdir?
    virtual QFuture<bool> remove(const QString &fileName) = 0;
    virtual QFuture<FileInfo> stat(const QString &fileName) = 0;

    static AbstractFileEngine *emptyEngine();

protected:
    AbstractFileEnginePrivate *d_ptr;

    friend class File;
};

#endif // ABSTRACTFILEENGINE_H
