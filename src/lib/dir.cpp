#include "dir.h"
#include "dir_p.h"

#include "file.h"
#include "pluginmanager_p.h"
#include "runextensions.h"

Dir::Dir() :
    d(new DirData)
{
    d->engine = AbstractDirEngine::emptyEngine();
}

Dir::Dir(const QUrl &url) :
    d(new DirData)
{
    d->url = url;
    d->engine = PluginManager::createDirEngine(url);
    if (!d->engine) {
        d->engine = AbstractDirEngine::emptyEngine();
    }
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

QFuture<bool> Dir::remove(const QString &fileName)
{
    return d->engine->remove(fileName);
}

QFuture<FileInfo> Dir::stat()
{
    return stat(".");
}

QFuture<FileInfo> Dir::stat(const QString &fileName)
{
    return d->engine->stat(fileName);
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

static bool doRemove(const QUrl &url)
{
    Dir dir(url);
    const auto filters = QDir::NoDotAndDotDot | QDir::AllEntries | QDir::Hidden | QDir::System;
    auto f1 = dir.entryList(filters);
    // TODO: wait for next result
    f1.waitForFinished();
    for (int i = 0; i < f1.resultCount(); ++i) {
        const FileInfo info = f1.resultAt(i);

        if (info.isDir())
            doRemove(info.url());

        auto f2 = dir.remove(info.fileName());
        f2.waitForFinished();
    }
    return true;
}

QFuture<bool> Dir::removeRecursively(const QUrl &url)
{
    typedef void (*func)(QFutureInterface<bool> &future, QUrl url);
    func f = [](QFutureInterface<bool> &future, QUrl url) {
        bool ok = doRemove(url);
        future.reportResult(ok);
    };
    return QtConcurrent::run(f, url);
}
