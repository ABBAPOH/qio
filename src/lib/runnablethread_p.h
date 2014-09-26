#ifndef RUNNABLETHREAD_P_H
#define RUNNABLETHREAD_P_H

#include "runnablethread.h"

#include <QtCore/QMutex>
#include <QtCore/QQueue>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>

class RunnableThreadPrivate : public QThread
{
public:
    QMutex mutex;
    QQueue<Runnable *> queue;
    QWaitCondition waitTask;
    QWaitCondition waitFinished;
    bool destroying;
    bool exiting;
    bool waitingForFinished;

protected:
    void run() Q_DECL_OVERRIDE;
};

#endif // RUNNABLETHREAD_P_H
