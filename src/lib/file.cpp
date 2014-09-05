#include "file.h"
#include "file_p.h"

#include "abstractfileengine.h"
#include "abstractfileengine_p.h"
#include "abstractfileengineplugin.h"
#include "pluginmanager_p.h"

#include <QtCore/QFutureWatcher>

void FilePrivate::init()
{
    state = File::Closed;
    openMode = QIODevice::NotOpen;
    size = 0;
}

File::File(QObject *parent) :
    QIODevice(parent),
    d_ptr(new FilePrivate(this))
{
    Q_D(File);
    d->init();
    d->engine = AbstractFileEngine::emptyEngine();
}

File::~File()
{
    Q_D(File);
    if (d->engine != AbstractFileEngine::emptyEngine())
        delete d->engine;
    delete d_ptr;
}

bool File::open(QIODevice::OpenMode mode)
{
    auto future = asyncOpen(mode);
    future.waitForFinished();
    return future.result();
}

QFuture<bool> File::asyncOpen(QIODevice::OpenMode mode)
{
    Q_D(File);
    if (d->state != File::Closed)
        return QFuture<bool>();
    d->state = File::Opening;
    d->openMode = mode;
    QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, &File::onOpenFinished);
    auto future = d->engine->open(mode);
    watcher->setFuture(future);
    return future;
}

void File::close()
{
    Q_D(File);
    d->size = 0;
    d->state = File::Closed;
    d->engine->close();
    QIODevice::close();
}

qint64 File::size() const
{
    Q_D(const File);
    return d->size;
}

bool File::seek(qint64 pos)
{
    Q_D(File);
    d->engine->seek(pos);
    return true;
}

qint64 File::bytesAvailable() const
{
    return 0;
}

qint64 File::bytesToWrite() const
{
    return 0;
}

bool File::waitForBytesWritten(int msecs)
{
    Q_D(File);
    return d->engine->waitForBytesWritten(msecs);
}

bool File::waitForReadyRead(int msecs)
{
    Q_D(File);
    return d->engine->waitForReadyRead(msecs);
}

QUrl File::url() const
{
    Q_D(const File);
    return d->url;
}

File::State File::state() const
{
    Q_D(const File);
    return d->state;
}

void File::setUrl(const QUrl &url)
{
    Q_D(File);
    if (d->url == url)
        return;

    d->url = url;
    d->engine = PluginManager::createEngine(url);
    if (!d->engine) {
        d->engine = AbstractFileEngine::emptyEngine();
        setErrorString(tr("Unsupported scheme %1").arg(url.scheme()));
    } else {
        d->engine->d_ptr->file = this;
    }
    emit urlChanged(url);
}

qint64 File::readData(char *data, qint64 maxlen)
{

}

qint64 File::writeData(const char *data, qint64 maxlen)
{

}

void File::onOpenFinished()
{
    Q_D(File);
    QFutureWatcher<bool> *watcher = static_cast<QFutureWatcher<bool> *>(sender());
    bool ok = watcher->future().result();
    if (ok) {
        d->state = File::Opened;
//        size = size;
        QIODevice::open(d->openMode);
    } else {
        d->state = File::Closed;
    }
    d->openMode = QIODevice::NotOpen;
    delete watcher;
}
