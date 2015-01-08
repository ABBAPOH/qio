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
    QSharedData(other),
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

ListJob FileEntry::list(QDir::Filters filters, QDir::SortFlags sortFlags)
{
    return d->engine->list(filters, sortFlags);
}

InfoListJob FileEntry::infoList(QDir::Filters filters, QDir::SortFlags sortFlags)
{
    return d->engine->infoList(filters, sortFlags);
}

FileJob FileEntry::mkdir(const QString &fileName)
{
    return d->engine->mkdir(fileName, false);
}

FileJob FileEntry::rmdir(const QString &fileName)
{
    return d->engine->rmdir(fileName, false);
}

FileJob FileEntry::mkpath(const QString &dirPath)
{
    return d->engine->mkdir(dirPath, true);
}

FileJob FileEntry::rmpath(const QString &dirPath)
{
    return d->engine->rmdir(dirPath, true);
}

FileJob FileEntry::touch(const QString &fileName)
{
    typedef void (*func)(QFutureInterface<FileResult> &future, QUrl url);
    func f = [](QFutureInterface<FileResult> &future, QUrl url) {
        File file(url);
        const bool ok = file.open(QIODevice::WriteOnly);
        future.reportResult(ok ? FileResult() : FileResult::Error::Unknown);
    };
    return QtConcurrent::run(f, absoluteUrl(this->url(), fileName));
}

FileJob FileEntry::remove(const QString &fileName)
{
    return d->engine->remove(fileName);
}

/*!
    Renames this file entry to the \a newName.
*/
FileJob FileEntry::rename(const QString &newName)
{
    return d->engine->rename(QString(), newName);
}

/*!
    Renames file or directory at \a oldName to the \a newName.
*/
FileJob FileEntry::rename(const QString &oldName, const QString &newName)
{
    return d->engine->rename(oldName, newName);
}

/*!
    Creates a link named linkName that points to this file.

    \note link can be created only on the same filesystem (if supported) and it
    is not possible to specify url to the other filesystem
*/
FileJob FileEntry::link(const QString &linkPath)
{
    Q_UNUSED(linkPath);
    Q_UNIMPLEMENTED();
    return FileJob();
}

StatJob FileEntry::stat(const QString &fileName)
{
    return d->engine->stat(fileName);
}

FileJob FileEntry::setPermissions(const QString &fileName, QFileDevice::Permissions permissions)
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
        auto f1 = dir.infoList(filters);
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

struct Operation
{
    enum Type { None = 0x0, Mkdir, RmDir, Copy, Remove };

    Operation() : type(None) {}
    Operation(Type type, const QUrl &source, const QUrl &destination = QUrl()) :
        type(type), source(source), destination(destination)
    {}

    Type type;
    QUrl source;
    QUrl destination;
};

enum OperationFlag { None = 0x0, Remove = 0x1, Copy = 0x2 };
Q_DECLARE_FLAGS(OperationFlags, OperationFlag)

static void operationListHelper(const FileInfo &info, const QUrl &destUrl, OperationFlags type, QVector<Operation> &result)
{
    static const auto filters = QDir::NoDotAndDotDot
            | QDir::AllEntries
            | QDir::Hidden
            | QDir::System;

    if (info.isDir()) {
        if (type & OperationFlag::Copy)
            result.append(Operation(Operation::Type::Mkdir, QUrl(), destUrl));

        FileEntry entry2(info.url());
        InfoListJob job2 = entry2.infoList(filters);
        job2.waitForFinished();
        foreach (const FileInfo &info3, job2.result()) {
            const QUrl destUrl2 = FileEntry::absoluteUrl(destUrl, info3.fileName());
            operationListHelper(info3, destUrl2, type, result);
        }

        if (type & OperationFlag::Remove)
            result.append(Operation(Operation::Type::RmDir, info.url()));

    } else {
        if (type & OperationFlag::Copy) {
            result.append(Operation(Operation::Type::Copy, info.url(), destUrl));
        }
        if (type & OperationFlag::Remove)
            result.append(Operation(Operation::Type::Remove, info.url()));
    }
}

static QVector<Operation> operationList(const QUrl &sourceUrl, const QUrl &destUrl, OperationFlags type)
{
    QVector<Operation> result;

    FileEntry entry(sourceUrl);
    StatJob job = entry.stat();
    job.waitForFinished();

    operationListHelper(job.result(), destUrl, type, result);
    return result;
}

static FileInfoList listToRemove(const QUrl &url)
{
    static const auto filters = QDir::NoDotAndDotDot
            | QDir::AllEntries
            | QDir::Hidden
            | QDir::System;

    FileEntry entry(url);
    StatJob job = entry.stat();
    job.waitForFinished();

    FileInfoList result;
    FileInfoList tmp;

    FileInfo info = job.result();
    tmp.append(info);

    while (!tmp.isEmpty()) {
        FileInfo info2 = tmp.takeFirst();
        result.prepend(info2);
        if (info2.isDir()) {
            FileEntry entry2(info2.url());
            InfoListJob job2 = entry2.infoList(filters);
            job2.waitForFinished();
            foreach (const FileInfo &info3, job2.result()) {
                tmp.prepend(info3);
            }
        }
    }

    return result;
}

FileResult copyFile(const QUrl &sourceUrl, const QUrl &destUrl)
{
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

    return FileResult();
}

void doWork(QFutureInterface<FileResult> &future, QUrl sourceUrl, QUrl destUrl, OperationFlags flags)
{
    const auto operations = ::operationList(sourceUrl, destUrl, flags);
    future.setProgressRange(0, operations.count());
    int count = 0;

    foreach (const Operation &op, operations) {
        FileEntry sourceEntry(op.source);
        FileEntry destEntry(op.destination);
        FileResult r;
        switch (op.type) {
        case Operation::Type::RmDir: {
            FileJob job = sourceEntry.rmdir();
            job.waitForFinished();
            r = job.result();
            break;
        } case Operation::Type::Remove: {
            FileJob job  = sourceEntry.remove();
            job.waitForFinished();
            r = job.result();
            break;
        } case Operation::Type::Copy: {
            r = copyFile(op.source, op.destination);
            Q_ASSERT(r);
            break;
        } case Operation::Type::Mkdir: {
            FileJob job = destEntry.mkdir();
            job.waitForFinished();
            r = job.result();
            break;
        } default:
            Q_UNREACHABLE();
            break;
        }

        if (!r) {
            future.reportResult(r);
            return;
        }

        future.setProgressValue(++count);
    }

    future.reportResult(FileResult());
}

FileJob FileEntry::removeRecursively(const QString &fileName)
{
    typedef void (*func)(QFutureInterface<FileResult> &future, QUrl url);
    func f = [](QFutureInterface<FileResult> &future, QUrl url) {
        doWork(future, url, QUrl(), OperationFlag::Remove);
        return;
    };
    return QtConcurrent::run(f, absoluteUrl(url(), fileName));
}

FileJob FileEntry::copy(const QUrl &destUrl)
{
    typedef void (*Handler)(QFutureInterface<FileResult> &, QUrl, QUrl);
    Handler func = [](QFutureInterface<FileResult> &future, QUrl sourceUrl, QUrl destUrl) {
        doWork(future, sourceUrl, destUrl, OperationFlag::Copy);
    };
    return QtConcurrent::run(func, absoluteUrl(url(), QString()), destUrl);
}

FileJob FileEntry::copy(const QString &fileName, const QUrl &destUrl)
{
    typedef void (*Handler)(QFutureInterface<FileResult> &, QUrl, QUrl);
    Handler func = [](QFutureInterface<FileResult> &future, QUrl sourceUrl, QUrl destUrl) {
        doWork(future, sourceUrl, destUrl, OperationFlag::Copy);
    };
    return QtConcurrent::run(func, absoluteUrl(url(), fileName), destUrl);
}

FileJob FileEntry::move(const QUrl &destUrl)
{
    typedef void (*Handler)(QFutureInterface<FileResult> &, QUrl, QUrl);
    Handler func = [](QFutureInterface<FileResult> &future, QUrl sourceUrl, QUrl destUrl) {
        OperationFlags flags(OperationFlag::Copy | OperationFlag::Remove);
        doWork(future, sourceUrl, destUrl, flags);
    };
    return QtConcurrent::run(func, absoluteUrl(url(), QString()), destUrl);
}

FileJob FileEntry::move(const QString &fileName, const QUrl &destUrl)
{
    typedef void (*Handler)(QFutureInterface<FileResult> &, QUrl, QUrl);
    Handler func = [](QFutureInterface<FileResult> &future, QUrl sourceUrl, QUrl destUrl) {
        OperationFlags flags(OperationFlag::Copy | OperationFlag::Remove);
        doWork(future, sourceUrl, destUrl, flags);
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
