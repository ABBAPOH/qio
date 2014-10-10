#include "direngine.h"

#include <QIO/FileInfoData>
#include <QIO/RunExtensions>

#include <QtConcurrent/QtConcurrentRun>

#include <QtCore/QDirIterator>
#include <QtCore/QFileInfo>
#include <QtCore/QMimeDatabase>

FileInfo fromQFileInfo(const QFileInfo &info)
{
    FileInfoData *d = new FileInfoData();

    d->url = QUrl::fromLocalFile(info.absoluteFilePath());
    d->exists = info.exists();
    d->isDir = info.isDir();
    d->size = info.size();
    d->created = info.created();
    d->lastModified = info.lastModified();
    d->lastRead = info.lastRead();
    d->mimeTypeName = QMimeDatabase().mimeTypeForUrl(d->url).name();

    return FileInfo(d);
}

DirEngine::DirEngine()
{
}

QFuture<QString> DirEngine::list(QDir::Filters filters)
{
    typedef void (*func)(QFutureInterface<QString> &future,
                         QString path,
                         QDir::Filters filters);
    func f = [](QFutureInterface<QString> &future,
            QString path,
            QDir::Filters filters) {
        QDirIterator it(path, filters);
        while (it.hasNext()) {
            it.next();
            future.reportResult(it.fileName());
        }
        return;
    };
    return QtConcurrent::run(f, url().toLocalFile(), filters);
}

QFuture<FileInfo> DirEngine::entryList(QDir::Filters filters)
{
    typedef void (*func)(QFutureInterface<FileInfo> &future,
                         QString path,
                         QDir::Filters filters);
    func f = [](QFutureInterface<FileInfo> &future,
            QString path,
            QDir::Filters filters) {
        QDirIterator it(path, filters);
        while (it.hasNext()) {
            it.next();
            future.reportResult(fromQFileInfo(it.fileInfo()));
        }
        return;
    };
    return QtConcurrent::run(f, url().toLocalFile(), filters);
}

QFuture<bool> DirEngine::mkdir(const QString &dirName)
{
    typedef void (*func)(QFutureInterface<bool> &future, QString path, QString dirName);
    func f = [](QFutureInterface<bool> &future, QString path, QString dirName) {
        future.reportResult(QDir(path).mkdir(dirName));
    };

    return QtConcurrent::run(f, url().toLocalFile(), dirName);
}

QFuture<bool> DirEngine::rmdir(const QString &dirName)
{
    typedef void (*func)(QFutureInterface<bool> &future, QString path, QString dirName);
    func f = [](QFutureInterface<bool> &future, QString path, QString dirName) {
        future.reportResult(QDir(path).rmdir(dirName));
    };

    return QtConcurrent::run(f, url().toLocalFile(), dirName);
}

QFuture<bool> DirEngine::remove(const QString &fileName)
{
    typedef void (*func)(QFutureInterface<bool> &future, QString path, QString fileName);
    func f = [](QFutureInterface<bool> &future, QString path, QString fileName) {
        future.reportResult(QDir(path).remove(fileName));
    };

    return QtConcurrent::run(f, url().toLocalFile(), fileName);
}

QFuture<FileInfo> DirEngine::stat(const QString &fileName)
{
    typedef void (*func)(QFutureInterface<FileInfo> &future, QString path, QString fileName);
    func f = [](QFutureInterface<FileInfo> &future, QString path, QString fileName) {
        future.reportResult(fromQFileInfo(QFileInfo(QDir(path).absoluteFilePath(fileName))));
    };

    return QtConcurrent::run(f, url().toLocalFile(), fileName);
}
