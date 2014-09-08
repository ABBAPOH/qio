#include "fileengineplugin.h"

#include "direngine.h"
#ifdef Q_OS_WIN
#include "fileenginewin.h"
#endif
#include "fileenginefallback.h"

FileEnginePlugin::FileEnginePlugin(QObject *parent) :
    AbstractFileEnginePlugin(parent)
{
}

QStringList FileEnginePlugin::schemes() const
{
    return QStringList() << "file" << "qrc";
}

AbstractFileEngine *FileEnginePlugin::createFileEngine(const QString &scheme) const
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

AbstractDirEngine *FileEnginePlugin::createDirEngine(const QString &scheme) const
{
    if (scheme != "qrc" || scheme != "file")
        return Q_NULLPTR;

    return new DirEngine;
}
