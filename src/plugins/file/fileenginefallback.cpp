#include "fileenginefallback.h"

#include <QIO/RunExtensions>
#include <QFutureWatcher>

FileEngineFallback::FileEngineFallback()
{
    data.file = new QFile();
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
    QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, &FileEngineFallback::onOpenFinished);
    openFuture = QtConcurrent::run(f, &data, mode);
    watcher->setFuture(openFuture);
}

bool FileEngineFallback::waitForOpened(int msecs)
{
    Q_UNUSED(msecs);
    openFuture.waitForFinished();
    return false;
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
    QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcherBase::finished, this, &FileEngineFallback::onReadFinished);
    watcher->setFuture(QtConcurrent::run(f, &data, maxlen));
}

bool FileEngineFallback::waitForBytesWritten(int msecs)
{
    Q_UNUSED(msecs);
    return false;
}

bool FileEngineFallback::waitForReadyRead(int msecs)
{
    Q_UNUSED(msecs);
    return false;
}

void FileEngineFallback::onOpenFinished()
{
    QFutureWatcher<bool> *watcher = static_cast<QFutureWatcher<bool> *>(sender());
    delete watcher;

    openFinished(openFuture.result());
    openFuture = QFuture<bool>();
}

void FileEngineFallback::onReadFinished()
{
    QFutureWatcher<void> *watcher = static_cast<QFutureWatcher<void> *>(sender());
    delete watcher;

    QMutexLocker l(&data.mutex);
    readFinished(data.readBuffer.constData(), data.readBuffer.length());
}
