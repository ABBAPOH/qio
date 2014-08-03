#include "abstractfileengineplugin.h"

#include "pluginmanager_p.h"
#include "abstractfileengine.h"

#include <QtCore/QMap>
#include <QtCore/QMutex>

AbstractFileEnginePlugin::AbstractFileEnginePlugin(QObject *parent) :
    QObject(parent)
{
}

AbstractFileEnginePlugin::~AbstractFileEnginePlugin()
{

}


