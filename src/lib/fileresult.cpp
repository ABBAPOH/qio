#include "fileresult.h"

QString FileResult::errorToString(Error error)
{
    switch (error) {
    case Error::None: return tr("No error");
    case Error::NoEntry: return tr("No such file or directory");
    case Error::NoAccess: return tr("Permission denied");
    default:
        break;
    }
    return tr("Unknown error");
}
