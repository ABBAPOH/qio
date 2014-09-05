#include "fileengine.h"

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

FileEngine::FileEngine()
{
}

QFuture<bool> FileEngine::open(QIODevice::OpenMode mode)
{
    Q_UNUSED(mode);
    return QFuture<bool>();
}

void FileEngine::close()
{

}

bool FileEngine::seek(qint64 pos)
{
    Q_UNUSED(pos);
    return false;
}

bool FileEngine::waitForBytesWritten(int msecs)
{
    Q_UNUSED(msecs);
    return false;
}

bool FileEngine::waitForReadyRead(int msecs)
{
    Q_UNUSED(msecs);
    return false;
}

QFuture<QString> FileEngine::list()
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
    return QtConcurrent::run(f, url().toLocalFile());
}

QFuture<FileInfo> FileEngine::entryList()
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
    return QtConcurrent::run(f, url().toLocalFile());
}

QFuture<bool> FileEngine::mkdir(const QString &dirName)
{
    typedef void (*func)(QFutureInterface<bool> &future, QString path, QString dirName);
    func f = [](QFutureInterface<bool> &future, QString path, QString dirName) {
        future.reportResult(QDir(path).mkdir(dirName));
    };

    return QtConcurrent::run(f, url().toLocalFile(), dirName);
}

QFuture<bool> FileEngine::remove(const QString &fileName)
{
    Q_UNUSED(fileName);
    return QFuture<bool>();
}

QFuture<FileInfo> FileEngine::stat(const QString &fileName)
{
    Q_UNUSED(fileName);
    return QFuture<FileInfo>();
}
