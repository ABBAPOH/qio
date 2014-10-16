#ifndef ARCHIVEPLUGIN_H
#define ARCHIVEPLUGIN_H

#include <QIO/AbstractFileEnginePlugin>

class ArchivePlugin : public AbstractFileEnginePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.arch.QIO.ArchivePlugin")
public:
    explicit ArchivePlugin(QObject *parent = 0);

    QStringList schemes() const Q_DECL_OVERRIDE;
    AbstractFileEngine *createFileEngine(const QString &scheme) const Q_DECL_OVERRIDE;
    AbstractDirEngine *createDirEngine(const QString &scheme) const Q_DECL_OVERRIDE;
};

#endif // ARCHIVEPLUGIN_H
