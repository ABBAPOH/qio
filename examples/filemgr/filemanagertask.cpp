#include "filemanagertask.h"

FileManagerTask::FileManagerTask(QObject *parent) :
    QObject(parent),
    m_state(StateNew),
    m_command(CommandNone),
    m_jobType(JobTypeUnknown)
{

}

void FileManagerTask::setCommand(FileManagerTask::Command cmd, const QVariant &data)
{
    m_command = cmd;
    m_commandData = QVariant::fromValue(data);
}

bool FileManagerTask::start()
{
    switch(m_jobType) {
    case JobTypeFile:
    {
        FileJob future;
        switch(m_command) {
        case CommandCreate:
        {
            FileEntry dir(m_srcPath);
            future = dir.mkdir(m_commandData.toString());
            break;
        }
        default:
            return false;

        }
        QFutureWatcher<FileResult> *watcher = new QFutureWatcher<FileResult>(this);
        connectWatcher(watcher);
        watcher->setFuture(future);
        break;
    }
    default:
        return false;
    }
    return true;
}

void FileManagerTask::changeState(FileManagerTask::State newState)
{
    m_state = newState;
    emit heartbeat();
}

void FileManagerTask::connectWatcher(const QFutureWatcherBase *watcher)
{
    connect(watcher, &QFutureWatcherBase::finished, this, &FileManagerTask::taskFinished);
    connect(watcher, &QFutureWatcherBase::canceled, this, &FileManagerTask::taskCanceled);
    connect(watcher, &QFutureWatcherBase::started, this, &FileManagerTask::taskInProgress);
    connect(watcher, &QFutureWatcherBase::resumed, this, &FileManagerTask::taskInProgress);
    connect(watcher, &QFutureWatcherBase::paused, this, &FileManagerTask::taskPaused);
}

void FileManagerTask::taskInProgress()
{
    changeState(StateProcessing);
}

void FileManagerTask::taskFinished()
{
    changeState(StateFinished);
}

void FileManagerTask::taskCanceled()
{
    changeState(StateCancelled);
}

void FileManagerTask::taskPaused()
{
    changeState(StatePaused);
}
