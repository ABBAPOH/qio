#include "fileengineplugin.h"

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

AbstractFileEngine *FileEnginePlugin::create(const QString &scheme) const
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
