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
    bufferSize = 10*1024*1024; // 10 Mb
    chunkSize = 4*1024; // 4 Kb
    engine = AbstractFileEngine::emptyEngine();
}

void FilePrivate::openFinished(bool ok)
{
    Q_Q(File);
    if (ok) {
        state = File::Opened;
//        size = size;
        q->QIODevice::open(openMode);
        if (openMode & QIODevice::Unbuffered)
            buffer.reserve(chunkSize);
        else
            buffer.reserve(bufferSize);
        if (openMode & QIODevice::ReadOnly) {
            qint64 maxlen = qMin<qint64>(chunkSize, q->size());
            engine->read(maxlen);
        }
    } else {
        state = File::Closed;
    }
    openMode = QIODevice::NotOpen;
}

void FilePrivate::readFinished(const char *data, qint64 length)
{
    Q_Q(File);
    if (length <= 0)
        return;

    int oldSize = buffer.size();
    buffer.resize(oldSize + length);
    memmove(buffer.data() + oldSize, data, length);
    qint64 maxlen = bufferSize - buffer.size();
    maxlen = qMin<qint64>(maxlen, chunkSize);
    maxlen = qMin(q->size() - (q->pos() + buffer.size()), maxlen);
    if (maxlen > 0)
        engine->read(maxlen);
    emit q->readyRead();
}

File::File(QObject *parent) :
    QIODevice(parent),
    d_ptr(new FilePrivate(this))
{
    Q_D(File);
    d->init();
}

File::File(const QUrl &url, QObject *parent) :
    QIODevice(parent),
    d_ptr(new FilePrivate(this))
{
    Q_D(File);
    d->init();

    setUrl(url);
}

File::~File()
{
    Q_D(File);
    close();
    if (d->engine != AbstractFileEngine::emptyEngine())
        delete d->engine;
    delete d_ptr;
}

bool File::open(QIODevice::OpenMode mode)
{
    Q_D(File);
    asyncOpen(mode);
    waitForOpened();
    return d->state == State::Opened;
}

void File::asyncOpen(QIODevice::OpenMode mode)
{
    Q_D(File);
    if (d->state != File::Closed)
        return;
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
    d->state = File::Closed;
    d->engine->close();
    QIODevice::close();
}

qint64 File::size() const
{
    Q_D(const File);
    return d->engine->size();
}

bool File::seek(qint64 pos)
{
    Q_D(File);
    QIODevice::seek(pos);
    return d->engine->seek(pos);
}

bool File::atEnd() const
{
    return pos() == size();
}

qint64 File::bytesAvailable() const
{
    Q_D(const File);
    return d->buffer.size();
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
    if (!isOpen())
        return false;
    if (bytesAvailable() > 0)
        return true;
    return d->engine->waitForReadyRead(msecs);
}

QUrl File::url() const
{
    Q_D(const File);
    return d->url;
}

void File::setUrl(const QUrl &url)
{
    Q_D(File);
    if (d->url == url)
        return;

    d->url = url;
    d->engine = PluginManager::createFileEngine(url);
    if (!d->engine) {
        d->engine = AbstractFileEngine::emptyEngine();
        setErrorString(tr("Unsupported scheme %1").arg(url.scheme()));
    } else {
        d->engine->d_ptr->file = this;
    }
    emit urlChanged(url);
}

int File::bufferSize() const
{
    Q_D(const File);
    return d->bufferSize;
}

void File::setBufferSize(int size)
{
    Q_D(File);
    if (d->bufferSize == size)
        return;

    d->bufferSize = size;
    emit bufferSizeChanged(size);
}

File::State File::state() const
{
    Q_D(const File);
    return d->state;
}

qint64 File::readData(char *data, qint64 maxlen)
{
    Q_D(File);

    maxlen = qMin(qint64(d->buffer.size()), maxlen);
    if (maxlen == 0)
        return 0;

    memmove(data, d->buffer.constData(), maxlen);
    d->buffer = d->buffer.mid(maxlen);

//    d->engine->read();

    return maxlen;
}

qint64 File::writeData(const char *data, qint64 maxlen)
{
    return -1;
}

void File::onOpenFinished()
{
    Q_D(File);
    QFutureWatcher<bool> *watcher = static_cast<QFutureWatcher<bool> *>(sender());
    bool ok = watcher->future().result();
    if (ok) {
        d->state = File::Opened;
//        size = size;
        QIODevice::open(d->openMode | QIODevice::Unbuffered);
//        if (d->openMode & QIODevice::Unbuffered)
//            d->buffer.reserve(d->chunkSize);
//        else
            d->buffer.reserve(d->bufferSize);
        if (d->openMode & QIODevice::ReadOnly)
            d->engine->read(d->chunkSize);
    } else {
        d->state = File::Closed;
    }
    d->openMode = QIODevice::NotOpen;
    delete watcher;
}
