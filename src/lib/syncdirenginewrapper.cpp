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

QFuture<QStringList> SyncDirEngineWrapper::list(QDir::Filters filters)
{
    typedef void (*Func)(QFutureInterface<QStringList> &, FileOperation, QDir::Filters);
    Func f = [](QFutureInterface<QStringList> &future, FileOperation op, QDir::Filters filters) {
        QMutexLocker l(&op.state->mutex);
        op.state->engine->setUrl(op.url);
        future.reportResult(op.state->engine->list(filters));
    };
    return QtConcurrent::run(f, FileOperation(m_state, url()), filters);
}

QFuture<FileInfoList> SyncDirEngineWrapper::entryList(QDir::Filters filters)
{
    typedef void (*Func)(QFutureInterface<FileInfoList> &, FileOperation, QDir::Filters);
    Func f = [](QFutureInterface<FileInfoList> &future, FileOperation op, QDir::Filters filters) {
        QMutexLocker l(&op.state->mutex);
        op.state->engine->setUrl(op.url);
        future.reportResult(op.state->engine->entryList(filters));
    };
    return QtConcurrent::run(f, FileOperation(m_state, url()), filters);
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
