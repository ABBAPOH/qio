#ifndef JOBS_H
#define JOBS_H

#include <QIO/FileInfo>
#include <QIO/FileResult>
#include <QtCore/QFuture>

class FileJob : public QFuture<FileResult>
{
public:
    inline FileJob() {}
    inline FileJob(const QFuture<FileResult> &other) : QFuture(other) {}

    inline FileJob &operator =(const QFuture<FileResult> &other)
    {
        QFuture::operator =(other);
        return *this;
    }
};

class StatJob: public QFuture<FileInfo>
{
public:
    StatJob() {}
    StatJob(const QFuture<FileInfo> &other) : QFuture(other) {}

    inline StatJob &operator =(const QFuture<FileInfo> &other)
    {
        QFuture::operator =(other);
        return *this;
    }
};

class ListJob: public QFuture<QStringList>
{
public:
    ListJob() {}
    ListJob(const QFuture<QStringList> &other) : QFuture(other) {}

    inline ListJob &operator =(const QFuture<QStringList> &other)
    {
        QFuture::operator =(other);
        return *this;
    }
};

class InfoListJob: public QFuture<FileInfoList>
{
public:
    InfoListJob() {}
    InfoListJob(const QFuture<FileInfoList> &other) : QFuture(other) {}

    inline InfoListJob &operator =(const QFuture<FileInfoList> &other)
    {
        QFuture::operator =(other);
        return *this;
    }
};

#endif // JOBS_H
