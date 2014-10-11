#include "abstractsyncdirengine.h"

AbstractSyncDirEngine::AbstractSyncDirEngine()
{
}

AbstractSyncDirEngine::~AbstractSyncDirEngine()
{
}

QUrl AbstractSyncDirEngine::url() const
{
    return m_url;
}

void AbstractSyncDirEngine::setUrl(const QUrl &url)
{
    m_url = url;
}
