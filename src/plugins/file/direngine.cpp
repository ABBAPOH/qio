#include "direngine.h"

#include <QIO/FileInfoData>
#include <QIO/RunnerHelpers>

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

DirEngine::DirEngine() :
    runner(new Runner)
{
}

DirEngine::~DirEngine()
{
    runner->waitForDone();
    delete runner;
}

QFuture<QString> DirEngine::list()
{
    typedef void (*func)(QFutureInterface<QString> &future, QString path);
    func f = [](QFutureInterface<QString> &future, QString path) {
        QDirIterator it(path, QDir::NoDotAndDotDot | QDir::AllEntries);
        while (it.hasNext()) {
            it.next();
            future.reportResult(it.fileName());
        }
        return;
    };
    return RunnerHelpers::run(runner, f, url().toLocalFile());
}

QFuture<FileInfo> DirEngine::entryList()
{
    typedef void (*func)(QFutureInterface<FileInfo> &future, QString path);
    func f = [](QFutureInterface<FileInfo> &future, QString path) {
        QDirIterator it(path, QDir::NoDotAndDotDot | QDir::AllEntries);
        while (it.hasNext()) {
            it.next();
            future.reportResult(fromQFileInfo(it.fileInfo()));
        }
        return;
    };
    return RunnerHelpers::run(runner, f, url().toLocalFile());
}

QFuture<bool> DirEngine::mkdir(const QString &dirName)
{
    typedef void (*func)(QFutureInterface<bool> &future, QString path, QString dirName);
    func f = [](QFutureInterface<bool> &future, QString path, QString dirName) {
        future.reportResult(QDir(path).mkdir(dirName));
    };

    return RunnerHelpers::run(runner, f, url().toLocalFile(), dirName);
}

QFuture<bool> DirEngine::remove(const QString &fileName)
{
    Q_UNUSED(fileName);
    return QFuture<bool>();
}

QFuture<FileInfo> DirEngine::stat(const QString &fileName)
{
    Q_UNUSED(fileName);
    return QFuture<FileInfo>();
}
