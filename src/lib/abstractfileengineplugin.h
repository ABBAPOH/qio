#ifndef ABSTRACTFILEENGINEHANDLER_H
#define ABSTRACTFILEENGINEHANDLER_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QUrl>

class AbstractFileEngine;

class AbstractFileEnginePlugin : public QObject
{
    Q_OBJECT
public:
    explicit AbstractFileEnginePlugin(QObject *parent = 0);
    ~AbstractFileEnginePlugin();

    virtual QStringList schemes() const = 0;
    virtual AbstractFileEngine *create(const QString &scheme) const = 0;
};

#endif // ABSTRACTFILEENGINEHANDLER_H
