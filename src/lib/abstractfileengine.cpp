#include "abstractfileengine.h"
#include "abstractfileengine_p.h"

#include "emptyfileengine.h"
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
