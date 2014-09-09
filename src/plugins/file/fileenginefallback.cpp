#include "fileenginefallback.h"

#include <QIO/RunExtensions>
#include <QFutureWatcher>

FileEngineFallback::FileEngineFallback(QObject *parent) :
    AbstractFileEngine(parent),
    openWatcher(new QFutureWatcher<bool>(this)),
    readWatcher(new QFutureWatcher<void>(this))
{
    data.file = new QFile();

    connect(openWatcher, &QFutureWatcherBase::finished, this, &FileEngineFallback::onOpenFinished);
    connect(readWatcher, &QFutureWatcherBase::finished, this, &FileEngineFallback::onReadFinished);
}

void FileEngineFallback::setUrl(const QUrl &url)
{
    AbstractFileEngine::setUrl(url);

    QMutexLocker l(&data.mutex);
    data.file->setFileName(url.toLocalFile());
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
    Q_UNUSED(pos);
    return false;
}

qint64 FileEngineFallback::size() const
{
    QMutexLocker l(&data.mutex);
    return data.file->size();
}

void FileEngineFallback::read(qint64 maxlen)
{
    typedef void (*func)(QFutureInterface<void> &future, ThreadData *data, qint64 maxlen);
    func f = [](QFutureInterface<void> &/*future*/, ThreadData *data, qint64 maxlen) {
        QMutexLocker l(&data->mutex);
        data->readBuffer.clear();
        data->readBuffer.resize(maxlen);
        maxlen = data->file->read(data->readBuffer.data(), maxlen);
        data->readBuffer.resize(maxlen);
    };
    readWatcher->setFuture(QtConcurrent::run(f, &data, maxlen));
}

bool FileEngineFallback::waitForBytesWritten(int msecs)
{
    Q_UNUSED(msecs);
    return false;
}

bool FileEngineFallback::waitForReadyRead(int msecs)
{
    Q_UNUSED(msecs);
    auto future = readWatcher->future();
    if (future == QFuture<bool>())
        return false;
    future.waitForFinished();
    onReadFinished();
    return false;
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
    auto future = readWatcher->future();
    if (future == QFuture<bool>())
        return;
    QMutexLocker l(&data.mutex);
    readFinished(data.readBuffer.constData(), data.readBuffer.length());
    readWatcher->setFuture(QFuture<bool>());
}
