#include "watchers.h"

/*!
    \class FileJobWatcher
*/

FileJobWatcher::FileJobWatcher(QObject *parent) :
    QFutureWatcher(parent)
{
}

/*!
    \class StatJobWatcher
*/

StatJobWatcher::StatJobWatcher(QObject *parent) :
    QFutureWatcher(parent)
{
}

/*!
    \class ListJobWatcher
*/

ListJobWatcher::ListJobWatcher(QObject *parent) :
    QFutureWatcher(parent)
{
}

/*!
    \class InfoListJobWatcher
*/

InfoListJobWatcher::InfoListJobWatcher(QObject *parent) :
    QFutureWatcher(parent)
{
}
