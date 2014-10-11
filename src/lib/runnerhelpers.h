#ifndef RUNNERHELPERS_H
#define RUNNERHELPERS_H

#include "runner.h"

#include <QtCore/QRunnable>
#include <QtCore/QFutureInterface>

namespace RunnerHelpers {

template <typename T,  typename FunctionPointer>
class StoredInterfaceFunctionCall0 : public QRunnable
{
public:
    StoredInterfaceFunctionCall0(Runner *runner, void (fn)(QFutureInterface<T> &)) :
        runner(runner), fn(fn)
    {
    }

    QFuture<T> start()
    {
        futureInterface.reportStarted();
        QFuture<T> future = futureInterface.future();
        runner->start(this);
        return future;
    }

    void run()
    {
        fn(futureInterface);
        futureInterface.reportFinished();
    }

private:
    Runner *runner;
    QFutureInterface<T> futureInterface;
    FunctionPointer fn;
};

template <typename T,  typename FunctionPointer, typename Arg1>
class StoredInterfaceFunctionCall1 : public QRunnable
{
public:
    StoredInterfaceFunctionCall1(Runner *runner, void (fn)(QFutureInterface<T> &, Arg1), const Arg1 &arg1) :
        runner(runner), fn(fn), arg1(arg1)
    {
    }

    QFuture<T> start()
    {
        futureInterface.reportStarted();
        QFuture<T> future = futureInterface.future();
        runner->start(this);
        return future;
    }

    void run()
    {
        fn(futureInterface, arg1);
        futureInterface.reportFinished();
    }

private:
    Runner *runner;
    QFutureInterface<T> futureInterface;
    FunctionPointer fn;
    Arg1 arg1;
};

template <typename T,  typename FunctionPointer, typename Arg1, typename Arg2>
class StoredInterfaceFunctionCall2 : public QRunnable
{
public:
    StoredInterfaceFunctionCall2(Runner *runner, void (fn)(QFutureInterface<T> &, Arg1, Arg2), const Arg1 &arg1, const Arg2 &arg2) :
        runner(runner), fn(fn), arg1(arg1), arg2(arg2)
    {
    }

    QFuture<T> start()
    {
        futureInterface.reportStarted();
        QFuture<T> future = futureInterface.future();
        runner->start(this);
        return future;
    }

    void run()
    {
        fn(futureInterface, arg1, arg2);
        futureInterface.reportFinished();
    }

private:
    Runner *runner;
    QFutureInterface<T> futureInterface;
    FunctionPointer fn;
    Arg1 arg1; Arg2 arg2;
};

template <typename T,  typename FunctionPointer, typename Arg1, typename Arg2, typename Arg3>
class StoredInterfaceFunctionCall3 : public QRunnable
{
public:
    StoredInterfaceFunctionCall3(Runner *runner, void (fn)(QFutureInterface<T> &, Arg1, Arg2, Arg3), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3) :
        runner(runner), fn(fn), arg1(arg1), arg2(arg2), arg3(arg3) { }

    QFuture<T> start()
    {
        futureInterface.reportStarted();
        QFuture<T> future = futureInterface.future();
        runner->start(this);
        return future;
    }

    void run()
    {
        fn(futureInterface, arg1, arg2, arg3);
        futureInterface.reportFinished();
    }

private:
    Runner *runner;
    QFutureInterface<T> futureInterface;
    FunctionPointer fn;
    Arg1 arg1; Arg2 arg2; Arg3 arg3;
};

template <typename T>
QFuture<T> run(Runner *runner, void (*functionPointer)(QFutureInterface<T> &))
{
    return (new StoredInterfaceFunctionCall0<T, void (*)(QFutureInterface<T> &)>(runner, functionPointer))->start();
}

template <typename T, typename Arg1>
QFuture<T> run(Runner *runner, void (*functionPointer)(QFutureInterface<T> &, Arg1), const Arg1 &arg1)
{
    return (new StoredInterfaceFunctionCall1<T, void (*)(QFutureInterface<T> &, Arg1), Arg1>(runner, functionPointer, arg1))->start();
}

template <typename T, typename Arg1, typename Arg2>
QFuture<T> run(Runner *runner, void (*functionPointer)(QFutureInterface<T> &, Arg1, Arg2), const Arg1 &arg1, const Arg2 &arg2)
{
    return (new StoredInterfaceFunctionCall2<T, void (*)(QFutureInterface<T> &, Arg1, Arg2), Arg1, Arg2>(runner, functionPointer, arg1, arg2))->start();
}

template <typename T, typename Arg1, typename Arg2, typename Arg3>
QFuture<T> run(Runner *runner, void (*functionPointer)(QFutureInterface<T> &, Arg1, Arg2, Arg3), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
{
    return (new StoredInterfaceFunctionCall3<T, void (*)(QFutureInterface<T> &, Arg1, Arg2, Arg3), Arg1, Arg2, Arg3>(runner, functionPointer, arg1, arg2, arg3))->start();
}

} // namespace RunnerHelpers

#endif // RUNNERHELPERS_H
