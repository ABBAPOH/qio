#include "syncdirenginewrapper.h"

#include "runextensions.h"

struct FileOperation
{
    FileOperation(SyncDirEngineWrapper::SharedStatePointer state, const QUrl &url, const QString &name = QString()) :
        state(state),
        url(url),
        name(name)
    {}

    SyncDirEngineWrapper::SharedStatePointer state;
    QUrl url;
    QString name;
};

typedef void (*Handler)(QFutureInterface<FileResult> &future, FileOperation operation);

SyncDirEngineWrapper::SyncDirEngineWrapper(AbstractSyncFileEntryEngine *engine) :
    m_state(new SharedState(engine))
{
}

class FileInfoComparator
{
    int flags;
public:
    explicit FileInfoComparator(QDir::SortFlags flags) : flags(flags) {}
    bool operator ()(const FileInfo &lhs, const FileInfo &rhs) const;
};

bool FileInfoComparator::operator ()(const FileInfo &lhs, const FileInfo &rhs) const
{
    if ((flags & QDir::DirsFirst) && (lhs.isDir() != rhs.isDir()))
        return lhs.isDir();
    if ((flags & QDir::DirsLast) && (lhs.isDir() != rhs.isDir()))
        return !lhs.isDir();

    int r = 0;
    int sortBy = (flags & QDir::SortByMask)
                 | (flags & QDir::Type);

    switch (sortBy) {
      case QDir::Time: {
        QDateTime firstModified = lhs.lastModified();
        QDateTime secondModified = rhs.lastModified();

        // QDateTime by default will do all sorts of conversions on these to
        // find timezones, which is incredibly expensive. As we aren't
        // presenting these to the user, we don't care (at all) about the
        // local timezone, so force them to UTC to avoid that conversion.
        firstModified.setTimeSpec(Qt::UTC);
        secondModified.setTimeSpec(Qt::UTC);

        r = firstModified.secsTo(secondModified);
        break;
      }
      case QDir::Size:
          r = int(qBound<qint64>(-1, rhs.size() - lhs.size(), 1));
        break;
      case QDir::Type:
      {
        const bool ic = flags & QDir::IgnoreCase;

        const QString leftType = ic ? lhs.mimeTypeName().toLower() : lhs.mimeTypeName();
        const QString rigthType = ic ? rhs.mimeTypeName().toLower() : rhs.mimeTypeName();

        r = flags & QDir::LocaleAware
            ? leftType.localeAwareCompare(rigthType)
            : leftType.compare(rigthType);
      }
        break;
      default:
        ;
    }

    if (r == 0 && sortBy != QDir::Unsorted) {
        // Still not sorted - sort by name
        bool ic = flags & QDir::IgnoreCase;

        const QString leftName = ic ? lhs.fileName().toLower() : lhs.fileName();
        const QString rightName = ic ? rhs.fileName().toLower() : rhs.fileName();

        r = flags & QDir::LocaleAware
            ? leftName.localeAwareCompare(rightName)
            : leftName.compare(rightName);
    }
    if (flags & QDir::Reversed)
        return r > 0;
    return r < 0;
}

static FileInfoList doList(FileOperation op, QDir::Filters filters, QDir::SortFlags sortFlags)
{
    QMutexLocker l(&op.state->mutex);
    op.state->engine->setUrl(op.url);
    FileInfoList list = op.state->engine->entryList(filters);
    if (!((sortFlags & QDir::SortByMask) == QDir::Unsorted))
        std::sort(list.begin(), list.end(), FileInfoComparator(sortFlags));
    return list;
}

QFuture<QStringList> SyncDirEngineWrapper::list(QDir::Filters filters, QDir::SortFlags sortFlags)
{
    typedef void (*Func)(QFutureInterface<QStringList> &,
                         FileOperation,
                         QDir::Filters,
                         QDir::SortFlags);
    Func f = [](QFutureInterface<QStringList> &future,
            FileOperation op,
            QDir::Filters filters,
            QDir::SortFlags sortFlags) {
        FileInfoList list = doList(op, filters, sortFlags);
        QStringList result;
        foreach (const FileInfo &info, list)
            result.append(info.fileName());
        future.reportResult(result);
    };
    return QtConcurrent::run(f, FileOperation(m_state, url()), filters, sortFlags);
}

QFuture<FileInfoList> SyncDirEngineWrapper::entryList(QDir::Filters filters,
                                                      QDir::SortFlags sortFlags)
{
    typedef void (*Func)(QFutureInterface<FileInfoList> &,
                         FileOperation,
                         QDir::Filters,
                         QDir::SortFlags);
    Func f = [](QFutureInterface<FileInfoList> &future,
            FileOperation op,
            QDir::Filters filters,
            QDir::SortFlags sortFlags) {
        future.reportResult(doList(op, filters, sortFlags));
    };
    return QtConcurrent::run(f, FileOperation(m_state, url()), filters, sortFlags);
}

QFuture<FileResult> SyncDirEngineWrapper::mkdir(const QString &dirName, bool createParents)
{
    typedef void (*Handler)(QFutureInterface<FileResult> &, FileOperation, bool);
    Handler h = [](QFutureInterface<FileResult> &future, FileOperation op, bool createParents) {
        QMutexLocker l(&op.state->mutex);
        op.state->engine->setUrl(op.url);
        future.reportResult(op.state->engine->mkdir(op.name, createParents));
    };

    return QtConcurrent::run(h, FileOperation(m_state, url(), dirName), createParents);
}

QFuture<FileResult> SyncDirEngineWrapper::rmdir(const QString &dirName, bool removeEmptyParents)
{
    typedef void (*Handler)(QFutureInterface<FileResult> &, FileOperation, bool);
    Handler h = [](QFutureInterface<FileResult> &future, FileOperation op, bool removeEmptyParents) {
        QMutexLocker l(&op.state->mutex);
        op.state->engine->setUrl(op.url);
        future.reportResult(op.state->engine->rmdir(op.name, removeEmptyParents));
    };

    return QtConcurrent::run(h, FileOperation(m_state, url(), dirName), removeEmptyParents);
}

QFuture<FileResult> SyncDirEngineWrapper::remove(const QString &fileName)
{
    Handler h = [](QFutureInterface<FileResult> &future, FileOperation op) {
        QMutexLocker l(&op.state->mutex);
        op.state->engine->setUrl(op.url);
        future.reportResult(op.state->engine->remove(op.name));
    };

    return QtConcurrent::run(h, FileOperation(m_state, url(), fileName));
}

QFuture<FileResult> SyncDirEngineWrapper::rename(const QString &oldName, const QString &newName)
{
    typedef void (*Handler)(QFutureInterface<FileResult> &, FileOperation, QString);
    Handler h = [](QFutureInterface<FileResult> &future, FileOperation op, QString newName) {
        QMutexLocker l(&op.state->mutex);
        op.state->engine->setUrl(op.url);
        future.reportResult(op.state->engine->rename(op.name, newName));
    };

    return QtConcurrent::run(h, FileOperation(m_state, url(), oldName), newName);
}

QFuture<FileResult> SyncDirEngineWrapper::setPermissions(const QString &fileName, QFile::Permissions permissions)
{
    typedef void (*Handler)(QFutureInterface<FileResult> &, FileOperation, QFile::Permissions);
    Handler h = [](QFutureInterface<FileResult> &future, FileOperation op, QFile::Permissions permissions) {
        QMutexLocker l(&op.state->mutex);
        op.state->engine->setUrl(op.url);
        future.reportResult(op.state->engine->setPermissions(op.name, permissions));
    };

    return QtConcurrent::run(h, FileOperation(m_state, url(), fileName), permissions);
}

QFuture<FileInfo> SyncDirEngineWrapper::stat(const QString &fileName)
{
    typedef void (*Handler)(QFutureInterface<FileInfo> &, FileOperation);
    Handler h = [](QFutureInterface<FileInfo> &future, FileOperation op) {
        QMutexLocker l(&op.state->mutex);
        op.state->engine->setUrl(op.url);
        future.reportResult(op.state->engine->stat(op.name));
    };

    return QtConcurrent::run(h, FileOperation(m_state, url(), fileName));
}
