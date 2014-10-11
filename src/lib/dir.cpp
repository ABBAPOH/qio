#include "dir.h"
#include "dir_p.h"

#include "file.h"
#include "pluginmanager_p.h"
#include "runextensions.h"

AbstractDirEngine *DirData::createEngine(const QUrl &url)
{
    AbstractDirEngine *engine = PluginManager::createDirEngine(url);
    if (!engine)
        engine = AbstractDirEngine::emptyEngine();
    return engine;
}

void DirData::destroyEngine(AbstractDirEngine *engine)
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

Dir::Dir() :
    d(new DirData)
{
    d->engine = AbstractDirEngine::emptyEngine();
}

Dir::Dir(const QUrl &url) :
    d(new DirData)
{
    d->url = url;
    d->engine = DirData::createEngine(url);
}

Dir::Dir(const Dir &rhs) :
    d(rhs.d)
{
}

Dir &Dir::operator=(const Dir &rhs)
{
    if (this != &rhs)
        d.operator=(rhs.d);
    return *this;
}

Dir::~Dir()
{
}

QUrl Dir::url() const
{
    return d->url;
}

void Dir::setUrl(const QUrl &url)
{
    if (d->url == url)
        return;

    if (d->url.scheme() != url.scheme()) {
        DirData::destroyEngine(d->engine);
        d->engine = DirData::createEngine(url);
    }
    d->url = url;
}

QFuture<QString> Dir::list(QDir::Filters filters)
{
    return d->engine->list(filters);
}

QFuture<FileInfo> Dir::entryList(QDir::Filters filters)
{
    return d->engine->entryList(filters);
}

QFuture<bool> Dir::mkdir(const QString &fileName)
{
    return d->engine->mkdir(fileName);
}

QFuture<bool> Dir::rmdir(const QString &fileName)
{
    return d->engine->rmdir(fileName);
}

QFuture<bool> Dir::rmdir(const QUrl &url)
{
    return Dir(getAbsoluteUrl(url)).rmdir(getFileName(url));
}

QFuture<bool> Dir::remove(const QString &fileName)
{
    return d->engine->remove(fileName);
}

QFuture<bool> Dir::remove(const QUrl &url)
{
    return Dir(getAbsoluteUrl(url)).remove(getFileName(url));
}

QFuture<FileInfo> Dir::stat()
{
    return stat(".");
}

QFuture<FileInfo> Dir::stat(const QString &fileName)
{
    return d->engine->stat(fileName);
}

QFuture<FileInfo> Dir::stat(const QUrl &url)
{
    return Dir(getAbsoluteUrl(url)).stat(getFileName(url));
}

QFuture<bool> Dir::touch(const QString &fileName)
{
    QUrl url = d->url;
    const QFileInfo info(url.path());
    url.setPath(info.absoluteFilePath() + "/" + fileName);
    return touch(url);
}

QFuture<bool> Dir::touch(const QUrl &url)
{
    typedef void (*func)(QFutureInterface<bool> &future, QUrl url);
    func f = [](QFutureInterface<bool> &future, QUrl url) {
        File file(url);
        const bool ok = file.open(QIODevice::WriteOnly);
        future.reportResult(ok);
    };
    return QtConcurrent::run(f, url);
}

static bool doRemove(const FileInfo &info)
{
    if (info.isDir()) {
        static const auto filters = QDir::NoDotAndDotDot
                | QDir::AllEntries
                | QDir::Hidden
                | QDir::System;
        const QUrl url = info.url();
        Dir dir(url);
        auto f1 = dir.entryList(filters);
        // TODO: wait for next result
        f1.waitForFinished();
        bool ok = true;
        for (int i = 0; i < f1.resultCount(); ++i)
            ok &= doRemove(f1.resultAt(i));

        if (!ok)
            return false;

        auto f = dir.rmdir(url);
        f.waitForFinished();
        return f.result();
    }

    auto f = Dir::remove(info.url());
    f.waitForFinished();
    return f.result();
}

QFuture<bool> Dir::removeRecursively(const QUrl &url)
{
    typedef void (*func)(QFutureInterface<bool> &future, QUrl url);
    func f = [](QFutureInterface<bool> &future, QUrl url) {
        auto f2 = Dir::stat(url);
        f2.waitForFinished();
        bool ok = doRemove(f2.result());
        future.reportResult(ok);
    };
    return QtConcurrent::run(f, url);
}

AbstractDirEngine *Dir::engine() const
{
    return d->engine;
}
