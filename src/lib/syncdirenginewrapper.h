#ifndef SYNCDIRENGINEWRAPPER_H
#define SYNCDIRENGINEWRAPPER_H

#include <QIO/AbstractDirEngine>
#include <QIO/AbstractSyncDirEngine>
#include <QIO/RunnerHelpers>

class QIO_EXPORT SyncDirEngineWrapper : public AbstractDirEngine
{
public:
    explicit SyncDirEngineWrapper(AbstractSyncDirEngine *engine);
    ~SyncDirEngineWrapper();

    QFuture<QString> list(QDir::Filters filters) Q_DECL_OVERRIDE;
    QFuture<FileInfo> entryList(QDir::Filters filters) Q_DECL_OVERRIDE;
    QFuture<bool> mkdir(const QString &dirName) Q_DECL_OVERRIDE;
    QFuture<bool> rmdir(const QString &dirName) Q_DECL_OVERRIDE;
    QFuture<bool> remove(const QString &fileName) Q_DECL_OVERRIDE;
    QFuture<FileInfo> stat(const QString &fileName) Q_DECL_OVERRIDE;

private:
    AbstractSyncDirEngine *m_engine;
    Runner *m_runner;
};

#endif // SYNCDIRENGINEWRAPPER_H
