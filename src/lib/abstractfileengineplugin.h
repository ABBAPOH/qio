#ifndef ABSTRACTFILEENGINEPLUGIN_H
#define ABSTRACTFILEENGINEPLUGIN_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QUrl>

class AbstractFileEngine;
class AbstractDirEngine;

class QIO_EXPORT AbstractFileEnginePlugin : public QObject
{
    Q_OBJECT
public:
    explicit AbstractFileEnginePlugin(QObject *parent = 0);
    ~AbstractFileEnginePlugin();

    virtual QStringList schemes() const = 0;
    virtual AbstractFileEngine *createFileEngine(const QString &scheme) const = 0;
    virtual AbstractDirEngine *createDirEngine(const QString &scheme) const = 0;
};

#endif // ABSTRACTFILEENGINEPLUGIN_H
