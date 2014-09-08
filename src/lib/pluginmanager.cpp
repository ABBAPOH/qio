#include "pluginmanager_p.h"

#include "abstractdirengine.h"
#include "abstractfileengine.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QPluginLoader>

Q_GLOBAL_STATIC(PluginManager, plugin_manager)

PluginManager::PluginManager()
{
    loadPlugins();
}

AbstractFileEngine *PluginManager::createFileEngine(const QUrl &url)
{
    AbstractFileEnginePlugin *handler = plugin_manager->handler(url.scheme());
    if (!handler)
        return 0;
    AbstractFileEngine *result = handler->createFileEngine(url.scheme());
    result->setUrl(url);
    return result;
}

AbstractDirEngine *PluginManager::createDirEngine(const QUrl &url)
{
    AbstractFileEnginePlugin *handler = plugin_manager->handler(url.scheme());
    if (!handler)
        return 0;
    AbstractDirEngine *result = handler->createDirEngine(url.scheme());
    result->setUrl(url);
    return result;
}

void PluginManager::loadPlugins()
{
    QDir dir = QCoreApplication::applicationDirPath();
    dir.cdUp();
    foreach (const QString &folder, QString(PLUGINS_DIR).split("/")) {
        dir.cd(folder);
    }
    qDebug() << "loading plugins from" << dir.absolutePath();
    foreach (const QString &fileName, dir.entryList(QDir::Files)) {
        QPluginLoader loader(dir.absoluteFilePath(fileName));
        AbstractFileEnginePlugin *plugin = qobject_cast<AbstractFileEnginePlugin *>(loader.instance());
        if (plugin) {
            foreach (const QString &scheme, plugin->schemes()) {
                handlers.insert(scheme, plugin);
            }
        }
    }
    qDebug() << "loaded shemes" << handlers.keys();
}

AbstractFileEnginePlugin *PluginManager::handler(const QString &scheme)
{
    // current implementation doesn't require locking as long as map is filled
    // in constructor
    return handlers.value(scheme);
}
