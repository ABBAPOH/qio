#ifndef DIR_P_H
#define DIR_P_H

#include "abstractdirengine.h"
#include "abstractfileengine.h"
#include "abstractfileengineplugin.h"
#include "fileentry.h"

#include <QtCore/QSharedData>

class FileEntryData : public QSharedData
{
public:
    FileEntryData();
    FileEntryData(const FileEntryData &other);
    ~FileEntryData();

    FileEntryData &operator =(const FileEntryData &other);

    QUrl url;
    AbstractDirEngine *engine;

    static AbstractDirEngine *createEngine(const QUrl &url);
    static void destroyEngine(AbstractDirEngine *engine);
};

#endif // DIR_P_H
