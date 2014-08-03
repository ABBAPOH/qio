#include "file.h"
#include "file_p.h"

#include "abstractfileengine.h"
#include "abstractfileengine_p.h"
#include "abstractfileengineplugin.h"
#include "pluginmanager_p.h"

void FilePrivate::init()
{
    state = File::Closed;
    openMode = QIODevice::NotOpen;
    size = 0;
}

void FilePrivate::openFinished(bool ok, qint64 size)
{
    Q_Q(File);
    if (ok) {
        state = File::Opened;
        size = size;
        q->QIODevice::open(openMode);
    } else {
        state = File::Closed;
    }
    openMode = QIODevice::NotOpen;
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
    asyncOpen(mode);
    return waitForOpened();
}

void File::asyncOpen(QIODevice::OpenMode mode)
{
    Q_D(File);
    d->state = File::Opening;
    d->openMode = mode;
    d->engine->open(mode);
}

bool File::waitForOpened(int msecs)
{
    Q_D(File);
    return d->engine->waitForOpened(msecs);
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
