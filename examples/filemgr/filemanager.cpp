#include "filemanager.h"

FileManager::FileManager(int &argc, char *argv[]) :
    QApplication(argc, argv)
{

}

void FileManager::update()
{
    qDebug("FileManager::update(): Tasks update (%i tasks)!", m_tasks.size());
    QMutableListIterator<FileManagerTask*> i(m_tasks);
    while(i.hasNext()) {
        FileManagerTask* task = i.next();
        switch(task->state()) {
        case FileManagerTask::StateNew:
            qDebug("FileManager::update(): Bad task: a task with StateNew is queued...");
            break;
        case FileManagerTask::StateQueued:
            //FIXME. Add a Queue manager here to resolve dependant tasks.
            qDebug("FileManager::update(): Starting a queued task (StateQueued)...");
            task->start();
            break;
        case FileManagerTask::StateFinished:
            qDebug("FileManager::update(): Found a finished task (StateFinished), recycling...");
            task->deleteLater();
            i.remove();
            break;
        case FileManagerTask::StateCancelled:
            //Don't do anything just yet.
        case FileManagerTask::StateProcessing:
        case FileManagerTask::StatePaused:
            break;
        default:
            qDebug("FileManager::update(): Found unknown state, WTF?");
            break;
        }
    }
}

void FileManager::mkdir(const QString &srcPath, const QString &dirName)
{
    //FIXME. Todo: Break this func in two.
    FileManagerTask *task = new FileManagerTask(this);
    connect(task, &FileManagerTask::heartbeat, this, &FileManager::update);
    m_tasks.append(task);
    task->setCommand(FileManagerTask::CommandCreate, dirName);
    task->setJobType(FileManagerTask::JobTypeFile);
    task->setSrcPath(srcPath);
    task->setDestPath(srcPath);
    task->setQueued();
}

