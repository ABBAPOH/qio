#include "fileresult.h"

FileResult::FileResult() :
    m_error(Error::None)
{
}

FileResult::FileResult(Error error) :
    m_error(error)
{
}

bool FileResult::hasError() const
{
    return m_error != Error::None;
}

FileResult::Error FileResult::error() const
{
    return m_error;
}

QString FileResult::errorString() const
{
    return errorToString(m_error);
}

void FileResult::swap(FileResult &other)
{
    qSwap(m_error, other.m_error);
}

QString FileResult::errorToString(Error error)
{
    switch (error) {
    case Error::None: return tr("No error");
    default:
        break;
    }
    return tr("Unknown error");
}
