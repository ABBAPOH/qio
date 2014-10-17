#include "abstractdirengine.h"
#include "emptyfileentryengine.h"

AbstractDirEngine::AbstractDirEngine()
{
}

AbstractDirEngine::~AbstractDirEngine()
{
}

QUrl AbstractDirEngine::url() const
{
    return m_url;
}

void AbstractDirEngine::setUrl(const QUrl &url)
{
    m_url = url;
}

Q_GLOBAL_STATIC(EmptyFileEntryEngine, empty_engine)

AbstractDirEngine *AbstractDirEngine::emptyEngine()
{
    return empty_engine();
}
