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

typedef void (*Handler)(QFutureInterface<bool> &future, FileOperation operation);

SyncDirEngineWrapper::SyncDirEngineWrapper(AbstractSyncDirEngine *engine) :
    m_state(new SharedState(engine))
{
}

QFuture<QString> SyncDirEngineWrapper::list(QDir::Filters filters)
{
    typedef void (*Func)(QFutureInterface<QString> &, FileOperation, QDir::Filters);
    Func f = [](QFutureInterface<QString> &future, FileOperation op, QDir::Filters filters) {
        QMutexLocker l(&op.state->mutex);
        op.state->engine->setUrl(op.url);
        const QStringList list = op.state->engine->list(filters);
        foreach (const QString &fileName, list) {
            future.reportResult(fileName);
        }
    };
    return QtConcurrent::run(f, FileOperation(m_state, url()), filters);
}

QFuture<FileInfo> SyncDirEngineWrapper::entryList(QDir::Filters filters)
{
    typedef void (*Func)(QFutureInterface<FileInfo> &, FileOperation, QDir::Filters);
    Func f = [](QFutureInterface<FileInfo> &future, FileOperation op, QDir::Filters filters) {
        QMutexLocker l(&op.state->mutex);
        op.state->engine->setUrl(op.url);
        const QList<FileInfo> list = op.state->engine->entryList(filters);
        foreach (const FileInfo &fileInfo, list) {
            future.reportResult(fileInfo);
        }
    };
    return QtConcurrent::run(f, FileOperation(m_state, url()), filters);
}

QFuture<bool> SyncDirEngineWrapper::mkdir(const QString &dirName)
{
    Handler h = [](QFutureInterface<bool> &future, FileOperation op) {
        QMutexLocker l(&op.state->mutex);
        op.state->engine->setUrl(op.url);
        future.reportResult(op.state->engine->mkdir(op.name));
    };

    return QtConcurrent::run(h, FileOperation(m_state, url(), dirName));
}

QFuture<bool> SyncDirEngineWrapper::rmdir(const QString &dirName)
{
    Handler h = [](QFutureInterface<bool> &future, FileOperation op) {
        QMutexLocker l(&op.state->mutex);
        op.state->engine->setUrl(op.url);
        future.reportResult(op.state->engine->rmdir(op.name));
    };

    return QtConcurrent::run(h, FileOperation(m_state, url(), dirName));
}

QFuture<bool> SyncDirEngineWrapper::remove(const QString &fileName)
{
    Handler h = [](QFutureInterface<bool> &future, FileOperation op) {
        QMutexLocker l(&op.state->mutex);
        op.state->engine->setUrl(op.url);
        future.reportResult(op.state->engine->remove(op.name));
    };

    return QtConcurrent::run(h, FileOperation(m_state, url(), fileName));
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
