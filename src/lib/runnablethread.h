#ifndef RUNNABLETHREAD_H
#define RUNNABLETHREAD_H

#include <QtCore/QObject>

class Runnable
{
    int ref;

    friend class RunnableThread;
    friend class RunnableThreadPrivate;

public:
    Runnable() : ref(0) { }
    virtual ~Runnable() { }

    virtual void run() = 0;

    bool autoDelete() const { return ref != -1; }
    void setAutoDelete(bool autoDelete) { ref = autoDelete ? 0 : -1; }
};

class RunnableThreadPrivate;
class QIO_EXPORT RunnableThread : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(RunnableThread)
    Q_DISABLE_COPY(RunnableThread)
public:
    explicit RunnableThread(QObject *parent = 0);
    ~RunnableThread();

public:
    void start(Runnable *runnable);
    void waitForDone();

protected:
    RunnableThreadPrivate *d_ptr;
};

#endif // RUNNABLETHREAD_H
