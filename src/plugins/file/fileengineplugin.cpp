#include "fileengineplugin.h"

#include "fileenginefallback.h"

FileEnginePlugin::FileEnginePlugin(QObject *parent) :
    AbstractFileEnginePlugin(parent)
{
}

QStringList FileEnginePlugin::schemes() const
{
    return QStringList() << "file";
}

AbstractFileEngine *FileEnginePlugin::create(const QString &scheme) const
{
    if (scheme != "file")
        return Q_NULLPTR;

    return new FileEngineFallback;
}
