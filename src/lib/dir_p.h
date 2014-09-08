#ifndef DIR_P_H
#define DIR_P_H

#include "abstractdirengine.h"
#include "abstractfileengine.h"
#include "abstractfileengineplugin.h"
#include "dir.h"

#include <QtCore/QSharedData>

class DirData : public QSharedData
{
public:
    QUrl url;
    AbstractDirEngine *engine;
};

#endif // DIR_P_H
