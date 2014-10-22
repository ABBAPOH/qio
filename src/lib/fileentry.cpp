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

QFuture<QStringList> FileEntry::list(QDir::Filters filters, QDir::SortFlags sortFlags)
{
    return d->engine->list(filters, sortFlags);
}

QFuture<FileInfoList> FileEntry::entryList(QDir::Filters filters, QDir::SortFlags sortFlags)
{
    return d->engine->entryList(filters, sortFlags);
}

QFuture<FileResult> FileEntry::mkdir(const QString &fileName)
{
    return d->engine->mkdir(fileName, false);
}

QFuture<FileResult> FileEntry::rmdir(const QString &fileName)
{
    return d->engine->rmdir(fileName, false);
}

QFuture<FileResult> FileEntry::mkpath(const QString &dirPath)
{
    return d->engine->mkdir(dirPath, true);
}

QFuture<FileResult> FileEntry::rmpath(const QString &dirPath)
{
    return d->engine->rmdir(dirPath, true);
}

QFuture<FileResult> FileEntry::touch(const QString &fileName)
{
    typedef void (*func)(QFutureInterface<FileResult> &future, QUrl url);
    func f = [](QFutureInterface<FileResult> &future, QUrl url) {
        File file(url);
        const bool ok = file.open(QIODevice::WriteOnly);
        future.reportResult(ok ? FileResult() : FileResult::Error::Unknown);
    };
    return QtConcurrent::run(f, absoluteUrl(this->url(), fileName));
}

QFuture<FileResult> FileEntry::remove(const QString &fileName)
{
    return d->engine->remove(fileName);
}

/*!
    Renames this file entry to the \a newName.
*/
QFuture<FileResult> FileEntry::rename(const QString &newName)
{
    return d->engine->rename(QString(), newName);
}

/*!
    Renames file or directory at \a oldName to the \a newName.
*/
QFuture<FileResult> FileEntry::rename(const QString &oldName, const QString &newName)
{
    return d->engine->rename(oldName, newName);
}

/*!
    Creates a link named linkName that points to this file.

    \note link can be created only on the same filesystem (if supported) and it
    is not possible to specify url to the other filesystem
*/
QFuture<FileResult> FileEntry::link(const QString &linkPath)
{
    Q_UNUSED(linkPath);
    Q_UNIMPLEMENTED();
    return QFuture<FileResult>();
}

QFuture<FileInfo> FileEntry::stat(const QString &fileName)
{
    return d->engine->stat(fileName);
}

QFuture<FileResult> FileEntry::setPermissions(const QString &fileName, QFileDevice::Permissions permissions)
{
    return d->engine->setPermissions(fileName, permissions);
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
        FileInfoList list = f1.result();
        bool ok = true;

        foreach (const auto &info, list)
            ok &= doRemove(info);

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

QFuture<FileResult> FileEntry::removeRecursively(const QString &fileName)
{
    typedef void (*func)(QFutureInterface<FileResult> &future, QUrl url);
    func f = [](QFutureInterface<FileResult> &future, QUrl url) {
        auto f2 = FileEntry(url).stat();
        f2.waitForFinished();
        bool ok = doRemove(f2.result());
        future.reportResult(ok ? FileResult() : FileResult::Error::Unknown);
    };
    return QtConcurrent::run(f, absoluteUrl(url(), fileName));
}

static FileResult doCopy(const QUrl &sourceUrl, const QUrl &destUrl)
{
    FileEntry sourceEntry(sourceUrl);
    auto statFuture = sourceEntry.stat();
    statFuture.waitForFinished();
    const FileInfo info = statFuture.result();

    if (!info.exists())
        return FileResult::Error::NoEntry;

    if (info.isDir()) {
        FileEntry destEntry(destUrl);
        auto mkdirFuture = destEntry.mkdir();
        mkdirFuture.waitForFinished();
        if (!mkdirFuture.result())
            return mkdirFuture.result();

        static const auto filters = QDir::NoDotAndDotDot
                | QDir::AllEntries
                | QDir::Hidden
                | QDir::System;
        auto listFuture = sourceEntry.entryList(filters);
        listFuture.waitForFinished();
        const FileInfoList list = listFuture.result();
        foreach (const FileInfo &childInfo, list) {
            QUrl childDestUrl = destUrl;
            childDestUrl.setPath(childDestUrl.path() + "/" + childInfo.fileName());
            FileResult result = doCopy(childInfo.url(), childDestUrl);
            if (!result)
                return result;
        }
    } else {
        File sourceFile(sourceUrl);
        File destFile(destUrl);
        if (!sourceFile.open(QIODevice::ReadOnly))
            return FileResult::Error::Unknown;
        if (!destFile.open(QIODevice::WriteOnly))
            return FileResult::Error::Unknown;

        while (!sourceFile.atEnd()) {
            sourceFile.waitForReadyRead();
            destFile.write(sourceFile.read(sourceFile.bytesAvailable()));
            destFile.waitForBytesWritten();
        }
    }

    return FileResult();
}

QFuture<FileResult> FileEntry::copy(const QUrl &destUrl)
{
    typedef void (*Handler)(QFutureInterface<FileResult> &, QUrl, QUrl);
    Handler func = [](QFutureInterface<FileResult> &future, QUrl sourceUrl, QUrl destUrl) {
        future.reportResult(doCopy(sourceUrl, destUrl));
    };
    return QtConcurrent::run(func, absoluteUrl(url(), QString()), destUrl);
}

QFuture<FileResult> FileEntry::copy(const QString &fileName, const QUrl &destUrl)
{
    typedef void (*Handler)(QFutureInterface<FileResult> &, QUrl, QUrl);
    Handler func = [](QFutureInterface<FileResult> &future, QUrl sourceUrl, QUrl destUrl) {
        future.reportResult(doCopy(sourceUrl, destUrl));
    };
    return QtConcurrent::run(func, absoluteUrl(url(), fileName), destUrl);
}

static FileResult doMove(const QUrl &sourceUrl, const QUrl &destUrl)
{
    FileResult result = doCopy(sourceUrl, destUrl);
    if (!result)
        return result;

    auto f2 = FileEntry(sourceUrl).stat();
    f2.waitForFinished();
    bool ok = doRemove(f2.result());
    return ok ? FileResult() : FileResult::Error::Unknown;
}

QFuture<FileResult> FileEntry::move(const QUrl &destUrl)
{
    typedef void (*Handler)(QFutureInterface<FileResult> &, QUrl, QUrl);
    Handler func = [](QFutureInterface<FileResult> &future, QUrl sourceUrl, QUrl destUrl) {
        future.reportResult(doMove(sourceUrl, destUrl));
    };
    return QtConcurrent::run(func, absoluteUrl(url(), QString()), destUrl);
}

QFuture<FileResult> FileEntry::move(const QString &fileName, const QUrl &destUrl)
{
    typedef void (*Handler)(QFutureInterface<FileResult> &, QUrl, QUrl);
    Handler func = [](QFutureInterface<FileResult> &future, QUrl sourceUrl, QUrl destUrl) {
        future.reportResult(doMove(sourceUrl, destUrl));
    };
    return QtConcurrent::run(func, absoluteUrl(url(), fileName), destUrl);
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
