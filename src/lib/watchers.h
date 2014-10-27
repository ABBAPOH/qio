#ifndef WATCHERS_H
#define WATCHERS_H

#include <QIO/Jobs>
#include <QtCore/QFutureWatcher>

class FileJobWatcher : public QFutureWatcher<FileJob>
{
    Q_OBJECT
    Q_DISABLE_COPY(FileJobWatcher)
public:
    explicit FileJobWatcher(QObject *parent = Q_NULLPTR);
};

class StatJobWatcher : public QFutureWatcher<StatJob>
{
    Q_OBJECT
    Q_DISABLE_COPY(StatJobWatcher)
public:
    explicit StatJobWatcher(QObject *parent = Q_NULLPTR);
};

class ListJobWatcher : public QFutureWatcher<ListJob>
{
    Q_OBJECT
    Q_DISABLE_COPY(ListJobWatcher)
public:
    explicit ListJobWatcher(QObject *parent = Q_NULLPTR);
};

class InfoListJobWatcher : public QFutureWatcher<InfoListJob>
{
    Q_OBJECT
    Q_DISABLE_COPY(InfoListJobWatcher)
public:
    explicit InfoListJobWatcher(QObject *parent = Q_NULLPTR);
};

#endif // WATCHERS_H
