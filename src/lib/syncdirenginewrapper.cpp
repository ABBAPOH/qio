#include "syncdirenginewrapper.h"

struct FileOperation
{
    FileOperation(AbstractSyncDirEngine *engine, const QUrl &url, const QString &name = QString()) :
        engine(engine),
        url(url),
        name(name)
    {}

    AbstractSyncDirEngine *engine;
    QUrl url;
    QString name;
};

typedef void (*Handler)(QFutureInterface<bool> &future, FileOperation operation);

SyncDirEngineWrapper::SyncDirEngineWrapper(AbstractSyncDirEngine *engine) :
    m_engine(engine),
    m_runner(new Runner)
{
}

SyncDirEngineWrapper::~SyncDirEngineWrapper()
{
    m_runner->waitForDone();
    delete m_engine;
    delete m_runner;
}

QFuture<QString> SyncDirEngineWrapper::list(QDir::Filters filters)
{
    typedef void (*Func)(QFutureInterface<QString> &, FileOperation, QDir::Filters);
    Func f = [](QFutureInterface<QString> &future, FileOperation op, QDir::Filters filters) {
        op.engine->setUrl(op.url);
        const QStringList list = op.engine->list(filters);
        foreach (const QString &fileName, list) {
            future.reportResult(fileName);
        }
    };
    return RunnerHelpers::run(m_runner, f, FileOperation(m_engine, url()), filters);
}

QFuture<FileInfo> SyncDirEngineWrapper::entryList(QDir::Filters filters)
{
    typedef void (*Func)(QFutureInterface<FileInfo> &, FileOperation, QDir::Filters);
    Func f = [](QFutureInterface<FileInfo> &future, FileOperation op, QDir::Filters filters) {
        op.engine->setUrl(op.url);
        const QList<FileInfo> list = op.engine->entryList(filters);
        foreach (const FileInfo &fileInfo, list) {
            future.reportResult(fileInfo);
        }
    };
    return RunnerHelpers::run(m_runner, f, FileOperation(m_engine, url()), filters);
}

QFuture<bool> SyncDirEngineWrapper::mkdir(const QString &dirName)
{
    Handler h = [](QFutureInterface<bool> &future, FileOperation op) {
        op.engine->setUrl(op.url);
        future.reportResult(op.engine->mkdir(op.name));
    };

    return RunnerHelpers::run(m_runner, h, FileOperation(m_engine, url(), dirName));
}

QFuture<bool> SyncDirEngineWrapper::rmdir(const QString &dirName)
{
    Handler h = [](QFutureInterface<bool> &future, FileOperation op) {
        op.engine->setUrl(op.url);
        future.reportResult(op.engine->rmdir(op.name));
    };

    return RunnerHelpers::run(m_runner, h, FileOperation(m_engine, url(), dirName));
}

QFuture<bool> SyncDirEngineWrapper::remove(const QString &fileName)
{
    Handler h = [](QFutureInterface<bool> &future, FileOperation op) {
        op.engine->setUrl(op.url);
        future.reportResult(op.engine->remove(op.name));
    };

    return RunnerHelpers::run(m_runner, h, FileOperation(m_engine, url(), fileName));
}

QFuture<FileInfo> SyncDirEngineWrapper::stat(const QString &fileName)
{
    typedef void (*Handler)(QFutureInterface<FileInfo> &, FileOperation);
    Handler h = [](QFutureInterface<FileInfo> &future, FileOperation op) {
        op.engine->setUrl(op.url);
        future.reportResult(op.engine->stat(op.name));
    };

    return RunnerHelpers::run(m_runner, h, FileOperation(m_engine, url(), fileName));
}
