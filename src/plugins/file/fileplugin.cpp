#include "fileplugin.h"

#include "syncdirengine.h"
#ifdef Q_OS_WIN
#include "fileenginewin.h"
#endif
#include "fileenginefallback.h"

#include <QIO/SyncDirEngineWrapper>

FilePlugin::FilePlugin(QObject *parent) :
    AbstractFileEnginePlugin(parent)
{
}

QStringList FilePlugin::schemes() const
{
    return QStringList() << "file" << "qrc";
}

AbstractFileEngine *FilePlugin::createFileEngine(const QString &scheme) const
{
    if (scheme == "qrc")
        return new FileEngineFallback;

    if (scheme != "file")
        return Q_NULLPTR;

#ifdef Q_OS_WIN
    return new FileEngineWin;
#else
    return new FileEngineFallback;
#endif
}

AbstractDirEngine *FilePlugin::createDirEngine(const QString &scheme) const
{
    if (scheme != "qrc" && scheme != "file")
        return Q_NULLPTR;

    return new SyncDirEngineWrapper(new SyncDirEngine);
}
