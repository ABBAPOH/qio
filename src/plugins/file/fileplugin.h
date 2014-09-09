#ifndef FILEPLUGIN_H
#define FILEPLUGIN_H

#include <QtCore/QtPlugin>
#include <QIO/AbstractFileEnginePlugin>

class FilePlugin : public AbstractFileEnginePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.arch.QIO.FilePlugin")
public:
    explicit FilePlugin(QObject *parent = 0);

    QStringList schemes() const Q_DECL_OVERRIDE;
    AbstractFileEngine *createFileEngine(const QString &scheme) const Q_DECL_OVERRIDE;
    AbstractDirEngine *createDirEngine(const QString &scheme) const Q_DECL_OVERRIDE;
};

#endif // FILEPLUGIN_H
