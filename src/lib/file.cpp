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

    skipNextRead = false;
}

void FilePrivate::openFinished(bool ok)
{
    Q_Q(File);
    if (ok) {
        state = File::Opened;
//        size = size;
        q->QIODevice::open(openMode);
        if (openMode & QIODevice::Unbuffered)
            readBuffer.reserve(chunkSize);
        else
            readBuffer.reserve(bufferSize);
        if (openMode & QIODevice::ReadOnly)
            postRead();
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

    state = File::State::Opened;
    const bool skip = skipNextRead;
    skipNextRead = false;
    if (!skip) { // if we had seek, we have to discard read data and read new chunk
        const int oldSize = readBuffer.size();
        readBuffer.resize(oldSize + length);
        memmove(readBuffer.data() + oldSize, data, length);
    }
    postRead();
    if (!skip)
        emit q->readyRead();
}

void FilePrivate::writeFinished(qint64 length)
{
    Q_Q(File);
    if (!writeBuffer.isEmpty()) // try to write newly came data
        postWrite();
    else if (q->openMode() & QIODevice::ReadOnly)
        postRead();
    emit q->bytesWritten(length);
}

void FilePrivate::postRead()
{
    Q_Q(File);
    qint64 maxlen = bufferSize - readBuffer.size();
    maxlen = qMin<qint64>(maxlen, chunkSize);
    maxlen = qMin(q->size() - (q->pos() + readBuffer.size()), maxlen);
    if (maxlen > 0) {
        engine->read(maxlen);
        state = File::State::Reading;
    }
}

void FilePrivate::postWrite()
{
    Q_ASSERT(!writeBuffer.isEmpty());
    engine->write(writeBuffer);
    writeBuffer.clear();
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
    return d->state >= State::Opened;
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
    const qint64 oldPos = this->pos();
    if (!QIODevice::seek(pos))
        return false;

    if (!d->engine->seek(pos)) {
        QIODevice::seek(oldPos);
        return false;
    }

    if (openMode() & QIODevice::ReadOnly) {
        if (d->state == State::Reading) {
            d->readBuffer.clear();
            d->skipNextRead = true;
        } else {
            d->postRead();
        }
    }

    return true;
}

bool File::atEnd() const
{
    return pos() == size();
}

qint64 File::bytesAvailable() const
{
    Q_D(const File);
    return d->readBuffer.size();
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
    if (d->state != Reading)
        return false;
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

//    qDebug() << this << "readData" << maxlen;
    maxlen = qMin(qint64(d->readBuffer.size()), maxlen);
    if (maxlen == 0)
        return 0;

    memmove(data, d->readBuffer.constData(), maxlen);
    d->readBuffer = d->readBuffer.mid(maxlen);

//    d->engine->read();

    return maxlen;
}

qint64 File::writeData(const char *data, qint64 maxlen)
{
    Q_D(File);
    const qint64 availableSize = d->bufferSize - d->writeBuffer.size();
    maxlen = qMin(maxlen, availableSize);
    if (maxlen == 0)
        return 0;

    const int oldSize = d->writeBuffer.size();
    d->writeBuffer.resize(oldSize + maxlen);
    memcpy(d->writeBuffer.data() + oldSize, data, maxlen);
    if (d->state == File::Opened)
        d->postWrite();
    return maxlen;
}
