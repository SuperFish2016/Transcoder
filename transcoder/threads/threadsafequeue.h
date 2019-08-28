#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H
#include <QQueue>
#include <QReadWriteLock>
#include <QReadLocker>
#include <QWriteLocker>
#include <QWaitCondition>
#include "../utils/buffer.h"
#include <QDebug>
#include <QMutexLocker>
#define QUEUE_SIZE 164
// 问题1，QWaitCondition 是不能喝QMutexLocker共同使用的
// 结论2，QReaderWriterLocker适合一个写线程和多个读线程的情形

class ThreadSafeQueue
{
public:
    explicit ThreadSafeQueue():
        enqueueStopped(false)
    {}

    int conut() const
    {
        QMutexLocker locker(&mutex);
        return queue.count();
    }

    bool isEmpty() const
    {
        QMutexLocker locker(&mutex);
        return queue.isEmpty();
    }

    void enqueueFinished()
    {
         QMutexLocker locker(&mutex);
         enqueueStopped = true;
         QueueIsNotEmpty.wakeAll(); //唤醒所有等待线程
         QueueIsNotFull.wakeOne();//唤醒读线程
    }

    void enqueue(TSR::FrameBuffer* data)
    {
        mutex.lock();
        if(queue.size() >= 64)
            QueueIsNotFull.wait(&mutex);// 此时queue队列已经满了，需要等待队列不满的信号，才能继续干活
        queue.enqueue(data);

        mutex.unlock();
        QueueIsNotEmpty.wakeAll();
    }

    TSR::FrameBuffer* dequeue()
    {
        mutex.lock();
        while(queue.isEmpty() && !enqueueStopped)
        {
            QueueIsNotEmpty.wait(&mutex);
        }
        TSR::FrameBuffer* data = NULL;
        if(!queue.isEmpty())
        {
            data = queue.dequeue();
        }
        mutex.unlock();
        QueueIsNotFull.wakeAll();
        return data;
    }

private:
    QQueue<TSR::FrameBuffer*> queue;
    mutable QMutex mutex;
    QWaitCondition QueueIsNotFull; //生产者可以往里面写数据
    QWaitCondition QueueIsNotEmpty;//消费者可以取数据，生产者写进数据后，就会释放QueueIsNotFull信号;
    volatile bool  enqueueStopped;//生产者在完成所有队列入队后，就会设置这个flag
};

typedef ThreadSafeQueue  DecodedFramesQueue;

#endif // THREADSAFEQUEUE_H
