#include "abstractfileengine.h"
#include "abstractfileengine_p.h"

#include "file_p.h"

AbstractFileEngine::AbstractFileEngine() :
    d_ptr(new AbstractFileEnginePrivate(this))
{
}

AbstractFileEngine::~AbstractFileEngine()
{
    delete d_ptr;
}

QUrl AbstractFileEngine::url() const
{
    Q_D(const AbstractFileEngine);
    return d->url;
}

void AbstractFileEngine::setUrl(const QUrl &url)
{
    Q_D(AbstractFileEngine);
    d->url = url;
}

void AbstractFileEngine::openFinished(bool ok, qint64 size)
{
    Q_D(AbstractFileEngine);
    if (d->file)
        d->file->d_ptr->openFinished(ok, size);
}

class EmptyFileEngine : public AbstractFileEngine
{
public:
    inline EmptyFileEngine() {}

    void open(QIODevice::OpenMode mode) Q_DECL_OVERRIDE;
    bool waitForOpened(int msecs = -1) Q_DECL_OVERRIDE;
    void close() Q_DECL_OVERRIDE;

    bool seek(qint64 pos) Q_DECL_OVERRIDE;

    bool waitForBytesWritten(int msecs = -1) Q_DECL_OVERRIDE;
    bool waitForReadyRead(int msecs = -1) Q_DECL_OVERRIDE;

    QFuture<QString> list() Q_DECL_OVERRIDE;
    QFuture<FileInfo> entryList() Q_DECL_OVERRIDE;
    QFuture<bool> mkdir(const QString &fileName) Q_DECL_OVERRIDE;
    // TODO: separate rmdir?
    QFuture<bool> remove(const QString &fileName) Q_DECL_OVERRIDE;
    QFuture<FileInfo> stat(const QString &fileName) Q_DECL_OVERRIDE;
};

void EmptyFileEngine::open(QIODevice::OpenMode mode)
{
    Q_UNUSED(mode);
}

bool EmptyFileEngine::waitForOpened(int msecs)
{
    Q_UNUSED(msecs);
    return false;
}

void EmptyFileEngine::close()
{

}

bool EmptyFileEngine::seek(qint64 pos)
{
    Q_UNUSED(pos);
    return false;
}

bool EmptyFileEngine::waitForBytesWritten(int msecs)
{
    Q_UNUSED(msecs);
    return false;
}

bool EmptyFileEngine::waitForReadyRead(int msecs)
{
    Q_UNUSED(msecs);
    return false;
}

QFuture<QString> EmptyFileEngine::list()
{
    return QFuture<QString>();
}

QFuture<FileInfo> EmptyFileEngine::entryList()
{
    return QFuture<FileInfo>();
}

QFuture<bool> EmptyFileEngine::mkdir(const QString &fileName)
{
    Q_UNUSED(fileName);
    return QFuture<bool>();
}

QFuture<bool> EmptyFileEngine::remove(const QString &fileName)
{
    Q_UNUSED(fileName);
    return QFuture<bool>();
}

QFuture<FileInfo> EmptyFileEngine::stat(const QString &fileName)
{
    Q_UNUSED(fileName);
    return QFuture<FileInfo>();
}

Q_GLOBAL_STATIC(EmptyFileEngine, empty_engine)

AbstractFileEngine *AbstractFileEngine::emptyEngine()
{
    return empty_engine();
}
