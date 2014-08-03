#ifndef ABSTRACTFILEENGINEHANDLER_P_H
#define ABSTRACTFILEENGINEHANDLER_P_H

#include "abstractfileengineplugin.h"

#include <QtCore/QMap>

class PluginManager
{
    Q_DISABLE_COPY(PluginManager)
public:
    PluginManager();

    static AbstractFileEngine *createEngine(const QUrl &url);

private:
    void loadPlugins();
    AbstractFileEnginePlugin *handler(const QString &scheme);

private:
    QMap<QString /*scheme*/, AbstractFileEnginePlugin *> handlers;
};

#endif // ABSTRACTFILEENGINEHANDLER_P_H
