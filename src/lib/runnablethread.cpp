#include "runnablethread.h"
#include "runnablethread_p.h"

RunnableThread::RunnableThread(QObject *parent) :
    QObject(parent),
    d_ptr(new RunnableThreadPrivate)
{
    Q_D(RunnableThread);
    d->destroying = false;
    d->exiting = false;
    d->waitingForFinished = false;
}

RunnableThread::~RunnableThread()
{
    Q_D(RunnableThread);
    d->destroying = true;
    {
        QMutexLocker l(&d->mutex);
        if (d->queue.isEmpty())
            d->waitTask.wakeOne();
    }
    d->wait();
    delete d_ptr;
}

void RunnableThread::start(Runnable *runnable)
{
    Q_D(RunnableThread);
    QMutexLocker l(&d->mutex);
    qDebug("start");
    if (runnable->autoDelete())
        runnable->ref++;
    d->queue.enqueue(runnable);
    if (d->isRunning()) {
        if (d->exiting) { // we're leaving from run() right now
            l.unlock();
            qDebug("wait");
            d->wait(); // wait for run() to end
            d->start(); // start it again
        } else {
            d->waitTask.wakeOne();
        }
    } else {
        d->start();
    }
}

void RunnableThread::waitForDone()
{
    Q_D(RunnableThread);
    if (!d->isRunning())
        return;
    QMutexLocker l(&d->mutex);
    d->waitingForFinished = true;
    d->waitFinished.wait(&d->mutex);
}

void RunnableThreadPrivate::run()
{
    QMutexLocker l(&mutex);
    qDebug("run");
    exiting = false;

    forever {
        if (!queue.isEmpty()) {
            Runnable *runnable = queue.dequeue();
            l.unlock();
            runnable->run();
            l.relock();
            if (runnable->autoDelete() && !--runnable->ref)
                delete runnable;
        } else {
            if (waitingForFinished) {
                waitingForFinished = false;
                waitFinished.wakeOne();
            }
            if (destroying)
                return;
            bool woken = waitTask.wait(&mutex, 5*1000);
            Q_UNUSED(woken);
            if (/*!woken*/ queue.isEmpty()) {
                qDebug("exiting");
                exiting = true;
                return;
            }
        }
    }
}
