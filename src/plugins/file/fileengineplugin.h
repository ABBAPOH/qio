#ifndef FILEENGINEPLUGIN_H
#define FILEENGINEPLUGIN_H

#include <QtCore/QtPlugin>
#include <QIO/AbstractFileEnginePlugin>

class FileEnginePlugin : public AbstractFileEnginePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.arch.QIO.FileEnginePlugin")
public:
    explicit FileEnginePlugin(QObject *parent = 0);

    QStringList schemes() const Q_DECL_OVERRIDE;
    AbstractFileEngine *create(const QString &scheme) const Q_DECL_OVERRIDE;
};

#endif // FILEENwGINEPLUGIN_H
