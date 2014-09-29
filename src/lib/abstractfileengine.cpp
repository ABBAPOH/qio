#include "abstractfileengine.h"
#include "abstractfileengine_p.h"

#include "file_p.h"

AbstractFileEngine::AbstractFileEngine(QObject *parent) :
    QObject(parent),
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

//void AbstractFileEngine::openFinished(bool ok, qint64 size)
//{
//    Q_D(AbstractFileEngine);
//    if (d->file)
//        d->file->d_ptr->openFinished(ok, size);
//}

class EmptyFileEngine : public AbstractFileEngine
{
public:
    inline EmptyFileEngine() {}

    void open(QIODevice::OpenMode mode) Q_DECL_OVERRIDE;
    bool waitForOpened(int msecs) Q_DECL_OVERRIDE;
    void close() Q_DECL_OVERRIDE;

    bool seek(qint64 pos) Q_DECL_OVERRIDE;
    qint64 size() const  Q_DECL_OVERRIDE;

    void read(qint64 maxlen) Q_DECL_OVERRIDE;
    void write(const QByteArray &data) Q_DECL_OVERRIDE;

    bool waitForBytesWritten(int msecs = -1) Q_DECL_OVERRIDE;
    bool waitForReadyRead(int msecs = -1) Q_DECL_OVERRIDE;
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

qint64 EmptyFileEngine::size() const
{
    return -1;
}

void EmptyFileEngine::read(qint64 maxlen)
{
    Q_UNUSED(maxlen);
}

void EmptyFileEngine::write(const QByteArray &data)
{
    Q_UNUSED(data);
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

Q_GLOBAL_STATIC(EmptyFileEngine, empty_engine)

AbstractFileEngine *AbstractFileEngine::emptyEngine()
{
    return empty_engine();
}

void AbstractFileEngine::openFinished(bool ok)
{
    Q_D(AbstractFileEngine);
    emit d->file->d_ptr->openFinished(ok);
}

void AbstractFileEngine::readFinished(const char *data, qint64 length)
{
    Q_D(AbstractFileEngine);
    d->file->d_ptr->readFinished(data, length);
}

void AbstractFileEngine::writeFinished(qint64 length)
{
    Q_D(AbstractFileEngine);
    d->file->d_ptr->writeFinished(length);
}
