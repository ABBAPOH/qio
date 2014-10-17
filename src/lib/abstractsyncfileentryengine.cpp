#include "abstractsyncfileentryengine.h"

AbstractSyncFileEntryEngine::AbstractSyncFileEntryEngine()
{
}

AbstractSyncFileEntryEngine::~AbstractSyncFileEntryEngine()
{
}

QUrl AbstractSyncFileEntryEngine::url() const
{
    return m_url;
}

void AbstractSyncFileEntryEngine::setUrl(const QUrl &url)
{
    m_url = url;
}
