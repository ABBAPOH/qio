#include "fileenginefallback.h"

#include <QIO/RunExtensions>

FileEngineFallback::FileEngineFallback()
{
    data.file = new QFile();
}

void FileEngineFallback::setUrl(const QUrl &url)
{
    FileEngine::setUrl(url);

    QMutexLocker l(&data.mutex);
    data.file->setFileName(url.toLocalFile());
}

QFuture<bool> FileEngineFallback::open(QIODevice::OpenMode mode)
{
    typedef void (*func)(QFutureInterface<bool> &future, ThreadData *data, QIODevice::OpenMode mode);
    func f = [](QFutureInterface<bool> &future, ThreadData *data, QIODevice::OpenMode mode) {
        QMutexLocker l(&data->mutex);
        bool ok = data->file->open(mode);
        future.reportResult(ok);
    };
    return QtConcurrent::run(f, &data, mode);
}

void FileEngineFallback::close()
{
    QMutexLocker l(&data.mutex);

    if (data.file)
        data.file->close();
}
