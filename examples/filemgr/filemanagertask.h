#ifndef FILEMANAGERTASK_H
#define FILEMANAGERTASK_H

#include <QIO/FileEntry>
#include <QIO/Jobs>
#include <QIO/Watchers>

class FileManagerTask : public QObject
{
    Q_OBJECT

public:
    enum State
    {
        StateNew = 0,
        StateFinished,
        StateCancelled,
        StateProcessing,
        StatePaused,
        StateQueued,
    };
    enum Command
    {
        CommandNone = 0,
        CommandCreate,
        CommandRemove,
        CommandRename,
        CommandQuery,
        CommandModify,
    };
    enum JobType
    {
        JobTypeUnknown = 0,
        JobTypeFile,
        JobTypeStat,
        JobTypeList,
        JobTypeInfoList,
    };

public:
    explicit FileManagerTask(QObject * parent = 0);

    State state() const { return m_state; }
    Command command() const { return m_command; }
    QString srcPath() const { return m_srcPath; }
    QString destPath() const { return m_destPath; }

    void setSrcPath(const QString &path) { m_srcPath = QString(path); }
    void setDestPath(const QString &path) { m_destPath = QString(path); }
    void setJobType(JobType jobType) { m_jobType = jobType; }
    void setCommand(Command cmd, const QVariant &data);
    void setQueued() { changeState(StateQueued); }

    bool start();
    void cancel();
    void pause();
    void resume();

private:
    void changeState(State newState);
    void connectWatcher(const QFutureWatcherBase *watcher);

public slots:
    void taskInProgress();
    void taskFinished();
    void taskCanceled();
    void taskPaused();

signals:
    void heartbeat();
    void startTask();
    void cancelTask();
    void pauseTask();
    void resumeTask();

private:
    State m_state;
    Command m_command;
    JobType m_jobType;
    QString m_srcPath;
    QString m_destPath;
    QVariant m_commandData;
};

#endif // FILEMANAGERTASK_H
