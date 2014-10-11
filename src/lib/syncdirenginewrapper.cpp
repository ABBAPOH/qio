#include "syncdirenginewrapper.h"

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
    typedef void (*func)(QFutureInterface<QString> &future,
                         AbstractSyncDirEngine *engine,
                         QUrl url,
                         QDir::Filters filters);
    func f = [](QFutureInterface<QString> &future,
            AbstractSyncDirEngine *engine,
            QUrl url,
            QDir::Filters filters) {
        engine->setUrl(url);
        const QStringList list = engine->list(filters);
        foreach (const QString &fileName, list) {
            future.reportResult(fileName);
        }
    };
    return RunnerHelpers::run(m_runner, f, m_engine, url(), filters);
}

QFuture<FileInfo> SyncDirEngineWrapper::entryList(QDir::Filters filters)
{
    typedef void (*func)(QFutureInterface<FileInfo> &future,
                         AbstractSyncDirEngine *engine,
                         QUrl url,
                         QDir::Filters filters);
    func f = [](QFutureInterface<FileInfo> &future,
            AbstractSyncDirEngine *engine,
            QUrl url,
            QDir::Filters filters) {
        engine->setUrl(url);
        const QList<FileInfo> list = engine->entryList(filters);
        foreach (const FileInfo &fileInfo, list) {
            future.reportResult(fileInfo);
        }
    };
    return RunnerHelpers::run(m_runner, f, m_engine, url(), filters);
}

QFuture<bool> SyncDirEngineWrapper::mkdir(const QString &dirName)
{
    typedef void (*func)(QFutureInterface<bool> &future,
                         AbstractSyncDirEngine *engine,
                         QUrl url,
                         QString dirName);
    func f = [](QFutureInterface<bool> &future,
            AbstractSyncDirEngine *engine,
            QUrl url,
            QString dirName) {
        engine->setUrl(url);
        future.reportResult(engine->mkdir(dirName));
    };

    return RunnerHelpers::run(m_runner, f, m_engine, url(), dirName);
}

QFuture<bool> SyncDirEngineWrapper::rmdir(const QString &dirName)
{
    typedef void (*func)(QFutureInterface<bool> &future,
                         AbstractSyncDirEngine *engine,
                         QUrl url,
                         QString dirName);
    func f = [](QFutureInterface<bool> &future,
            AbstractSyncDirEngine *engine,
            QUrl url,
            QString dirName) {
        engine->setUrl(url);
        future.reportResult(engine->rmdir(dirName));
    };

    return RunnerHelpers::run(m_runner, f, m_engine, url(), dirName);
}

QFuture<bool> SyncDirEngineWrapper::remove(const QString &fileName)
{
    typedef void (*func)(QFutureInterface<bool> &future,
                         AbstractSyncDirEngine *engine,
                         QUrl url,
                         QString fileName);
    func f = [](QFutureInterface<bool> &future,
            AbstractSyncDirEngine *engine,
            QUrl url,
            QString fileName) {
        engine->setUrl(url);
        future.reportResult(engine->remove(fileName));
    };

    return RunnerHelpers::run(m_runner, f, m_engine, url(), fileName);
}

QFuture<FileInfo> SyncDirEngineWrapper::stat(const QString &fileName)
{
    typedef void (*func)(QFutureInterface<FileInfo> &future,
                         AbstractSyncDirEngine *engine,
                         QUrl url,
                         QString fileName);
    func f = [](QFutureInterface<FileInfo> &future,
            AbstractSyncDirEngine *engine,
            QUrl url,
            QString fileName) {
        engine->setUrl(url);
        future.reportResult(engine->stat(fileName));
    };

    return RunnerHelpers::run(m_runner, f, m_engine, url(), fileName);
}
