#include "dir.h"
#include "dir_p.h"

#include "pluginmanager_p.h"

Dir::Dir() :
    d(new DirData)
{
    d->engine = AbstractFileEngine::emptyEngine();
}

Dir::Dir(const QUrl &url) :
    d(new DirData)
{
    d->url = url;
    d->engine = PluginManager::createEngine(url);
    if (!d->engine) {
        d->engine = AbstractFileEngine::emptyEngine();
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

QFuture<QString> Dir::list()
{
    return d->engine->list();
}

QFuture<FileInfo> Dir::entryList()
{
    return d->engine->entryList();
}

QFuture<bool> Dir::mkdir(const QString &fileName)
{
    return d->engine->mkdir(fileName);
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
