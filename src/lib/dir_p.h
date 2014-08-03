#ifndef DIR_P_H
#define DIR_P_H

#include "abstractfileengine.h"
#include "abstractfileengineplugin.h"
#include "dir.h"

#include <QtCore/QSharedData>

class DirData : public QSharedData
{
public:
    QUrl url;
    AbstractFileEngine *engine;
};

#endif // DIR_P_H
