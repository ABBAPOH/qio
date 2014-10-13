#include "fileentry.h"
#include "fileentry_p.h"

#include "file.h"
#include "pluginmanager_p.h"
#include "runextensions.h"

FileEntryData::FileEntryData() :
    engine(Q_NULLPTR)
{
}

FileEntryData::FileEntryData(const FileEntryData &other) :
    url(other.url),
    engine(createEngine(other.url))
{
}

FileEntryData::~FileEntryData()
{
    destroyEngine(engine);
}

FileEntryData &FileEntryData::operator =(const FileEntryData &other)
{
    url = other.url;
    destroyEngine(engine);
    engine = createEngine(other.url);
    return *this;
}

AbstractDirEngine *FileEntryData::createEngine(const QUrl &url)
{
    AbstractDirEngine *engine = PluginManager::createDirEngine(url);
    if (!engine)
        engine = AbstractDirEngine::emptyEngine();
    return engine;
}

void FileEntryData::destroyEngine(AbstractDirEngine *engine)
{
    if (engine != AbstractDirEngine::emptyEngine())
        delete engine;
}

static inline QString getAbsolutePath(const QUrl &url)
{
    return QFileInfo(url.path()).absolutePath();
}

static inline QUrl getAbsoluteUrl(const QUrl &url)
{
    return QUrl::fromLocalFile(getAbsolutePath(url));
}

static inline QString getFileName(const QUrl &url)
{
    return QFileInfo(url.path()).fileName();
}

FileEntry::FileEntry() :
    d(new FileEntryData)
{
    d->engine = AbstractDirEngine::emptyEngine();
}

FileEntry::FileEntry(const QUrl &url) :
    d(new FileEntryData)
{
    d->url = url;
    d->engine = FileEntryData::createEngine(url);
}

FileEntry::FileEntry(const FileEntry &rhs) :
    d(rhs.d)
{
}

FileEntry &FileEntry::operator=(const FileEntry &rhs)
{
    if (this != &rhs)
        d.operator=(rhs.d);
    return *this;
}

FileEntry::~FileEntry()
{
}

QUrl FileEntry::url() const
{
    return d->url;
}

void FileEntry::setUrl(const QUrl &url)
{
    if (d->url == url)
        return;

    if (d->url.scheme() != url.scheme()) {
        FileEntryData::destroyEngine(d->engine);
        d->engine = FileEntryData::createEngine(url);
    }
    d->url = url;
}

QFuture<QString> FileEntry::list(QDir::Filters filters)
{
    return d->engine->list(filters);
}

QFuture<FileInfo> FileEntry::entryList(QDir::Filters filters)
{
    return d->engine->entryList(filters);
}

QFuture<bool> FileEntry::mkdir(const QString &fileName)
{
    return d->engine->mkdir(fileName);
}

QFuture<bool> FileEntry::rmdir(const QString &fileName)
{
    return d->engine->rmdir(fileName);
}

QFuture<bool> FileEntry::remove(const QString &fileName)
{
    return d->engine->remove(fileName);
}

QFuture<FileInfo> FileEntry::stat(const QString &fileName)
{
    return d->engine->stat(fileName);
}

QFuture<bool> FileEntry::touch(const QString &fileName)
{
    typedef void (*func)(QFutureInterface<bool> &future, QUrl url);
    func f = [](QFutureInterface<bool> &future, QUrl url) {
        File file(url);
        const bool ok = file.open(QIODevice::WriteOnly);
        future.reportResult(ok);
    };
    return QtConcurrent::run(f, absoluteUrl(this->url(), fileName));
}

static bool doRemove(const FileInfo &info)
{
    if (info.isDir()) {
        static const auto filters = QDir::NoDotAndDotDot
                | QDir::AllEntries
                | QDir::Hidden
                | QDir::System;
        const QUrl url = info.url();
        FileEntry dir(url);
        auto f1 = dir.entryList(filters);
        // TODO: wait for next result
        f1.waitForFinished();
        bool ok = true;
        for (int i = 0; i < f1.resultCount(); ++i)
            ok &= doRemove(f1.resultAt(i));

        if (!ok)
            return false;

        auto f = dir.rmdir();
        f.waitForFinished();
        return f.result();
    }

    auto f = FileEntry(info.url()).remove();
    f.waitForFinished();
    return f.result();
}

QFuture<bool> FileEntry::removeRecursively(const QString &fileName)
{
    typedef void (*func)(QFutureInterface<bool> &future, QUrl url);
    func f = [](QFutureInterface<bool> &future, QUrl url) {
        auto f2 = FileEntry(url).stat();
        f2.waitForFinished();
        bool ok = doRemove(f2.result());
        future.reportResult(ok);
    };
    return QtConcurrent::run(f, absoluteUrl(url(), fileName));
}

/*!
    Appends \a relativePath to the \a parentUrl's path and returns new url.
*/
QUrl FileEntry::absoluteUrl(const QUrl &parentUrl, const QString &relativePath)
{
    if (relativePath.isEmpty())
        return parentUrl;
    QUrl result = parentUrl;
    const QString path = QDir::cleanPath(result.path() + "/" + relativePath);
    result.setPath(path);
    return result;
}

AbstractDirEngine *FileEntry::engine() const
{
    return d->engine;
}
