#include "fileenginefallback.h"

#include <QIO/RunExtensions>
#include <QFutureWatcher>


FileEngineFallback::FileEngineFallback(QObject *parent) :
    AbstractFileEngine(parent),
    openWatcher(new QFutureWatcher<bool>(this)),
    readWatcher(new QFutureWatcher<QByteArray>(this)),
    writeWatcher(new QFutureWatcher<qint64>(this))
{
    data.file = new QFile();

    connect(openWatcher, &QFutureWatcherBase::finished, this, &FileEngineFallback::onOpenFinished);
    connect(readWatcher, &QFutureWatcherBase::finished, this, &FileEngineFallback::onReadFinished);
    connect(writeWatcher, &QFutureWatcherBase::finished, this, &FileEngineFallback::onWriteFinished);
}

void FileEngineFallback::setUrl(const QUrl &url)
{
    AbstractFileEngine::setUrl(url);

    QMutexLocker l(&data.mutex);
    const QString path = url.toLocalFile();
    data.file->setFileName(path);
    fileInfo.setFile(path);
}

void FileEngineFallback::open(QIODevice::OpenMode mode)
{
    typedef void (*func)(QFutureInterface<bool> &future, ThreadData *data, QIODevice::OpenMode mode);
    func f = [](QFutureInterface<bool> &future, ThreadData *data, QIODevice::OpenMode mode) {
        QMutexLocker l(&data->mutex);
        bool ok = data->file->open(mode);
        future.reportResult(ok);
    };
    openWatcher->setFuture(QtConcurrent::run(f, &data, mode));
}

bool FileEngineFallback::waitForOpened(int msecs)
{
    Q_UNUSED(msecs);
    auto future = openWatcher->future();
    if (future == QFuture<bool>())
        return false;
    future.waitForFinished();
    onOpenFinished();
    return true;
}

void FileEngineFallback::close()
{
    QMutexLocker l(&data.mutex);

    if (data.file)
        data.file->close();
}

bool FileEngineFallback::seek(qint64 pos)
{
    QMutexLocker l(&data.mutex);
    return data.file->seek(pos);
}

qint64 FileEngineFallback::size() const
{
    fileInfo.refresh();
    return fileInfo.size();
}

void FileEngineFallback::read(qint64 maxlen)
{
    typedef void (*func)(QFutureInterface<QByteArray> &future, ThreadData *data, qint64 maxlen);
    func f = [](QFutureInterface<QByteArray> &future, ThreadData *data, qint64 maxlen) {
        QMutexLocker l(&data->mutex);
        QByteArray buffer;
        buffer.resize(maxlen);
        maxlen = data->file->read(buffer.data(), maxlen);
        buffer.resize(maxlen);
        future.reportResult(buffer);
    };
    Q_ASSERT(!reading);
    reading = true;
    readWatcher->setFuture(QtConcurrent::run(f, &data, maxlen));
}

void FileEngineFallback::write(const QByteArray &data)
{
    typedef void (*func)(QFutureInterface<qint64> &future, ThreadData *d, QByteArray data);
    func f = [](QFutureInterface<qint64> &future, ThreadData *d, QByteArray data) {
        QMutexLocker l(&d->mutex);
        const qint64 written = d->file->write(data);
        future.reportResult(written);
    };
    writeWatcher->setFuture(QtConcurrent::run(f, &this->data, data));
}

bool FileEngineFallback::waitForBytesWritten(int msecs)
{
    Q_UNUSED(msecs);
    return false;
}

bool FileEngineFallback::waitForReadyRead(int msecs)
{
    Q_UNUSED(msecs);
    if (!reading)
        return false;
    auto future = readWatcher->future();
    future.waitForFinished();
    onReadFinished();
    return true;
}

void FileEngineFallback::onOpenFinished()
{
    auto future = openWatcher->future();
    if (future == QFuture<bool>())
        return;
    openFinished(future.result());
    openWatcher->setFuture(QFuture<bool>());
}

void FileEngineFallback::onReadFinished()
{
    if (!reading)
        return;
    reading = false;
    auto buffer = readWatcher->future().result();
    readFinished(buffer.constData(), buffer.length());
}

void FileEngineFallback::onWriteFinished()
{
    const qint64 written = writeWatcher->future().result();
    writeFinished(written);
}
