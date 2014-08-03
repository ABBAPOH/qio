#ifndef ABSTRACTFILEENGINE_P_H
#define ABSTRACTFILEENGINE_P_H

#include "abstractfileengine.h"

class AbstractFileEnginePrivate
{
    Q_DECLARE_PUBLIC(AbstractFileEngine)
    AbstractFileEngine *q_ptr;

public:
    explicit AbstractFileEnginePrivate(AbstractFileEngine *qq) : q_ptr(qq) {}

    QUrl url;
    File *file;
};

#endif // ABSTRACTFILEENGINE_P_H
