#include "archiveplugin.h"

#include "archiveentryengine.h"
#include <QIO/SyncDirEngineWrapper>

ArchivePlugin::ArchivePlugin(QObject *parent) :
    AbstractFileEnginePlugin(parent)
{
}

QStringList ArchivePlugin::schemes() const
{
    return QStringList() << "zip";
}

AbstractFileEngine *ArchivePlugin::createFileEngine(const QString &scheme) const
{
    return Q_NULLPTR;
}

AbstractDirEngine *ArchivePlugin::createDirEngine(const QString &scheme) const
{
    if (scheme != "zip")
        return Q_NULLPTR;
    return new SyncDirEngineWrapper(new ArchiveEntryEngine);
}
