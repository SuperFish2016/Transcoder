#ifndef THREADSAFEVECTOR_H
#define THREADSAFEVECTOR_H
#include <QVector>
#include <QMutexLocker>
#include <QMutex>
#include <QWaitCondition>
#include "../utils/buffer.h"
#include <QDebug>
#define  J2KFramesVectorSize 164
// 问题1，QWaitCondition 是不能喝QMutexLocker共同使用的
// 结论2，QReaderWriterLocker适合一个写线程和多个读线程的情形
// 此处Vector用于存放编码后的J2K数据，供写线程调用
class ThreadSafeVector
{
public:
    explicit ThreadSafeVector(){}

    int conut() const
    {
        QMutexLocker locker(&lock);
        return vector.count();
    }

    bool isEmpty() const
    {
        QMutexLocker locker(&lock);
        return vector.isEmpty();
    }

    void pushData(TSR::FrameBuffer* buffer)
    {
        QMutexLocker locker(&lock);
        while(vector.count() >= J2KFramesVectorSize)
        {
            VectorIsNotFull.wait(&lock);// 此时queue队列已经满了，需要等待队列不满的信号，才能继续干活
        }
        vector.push_back(buffer);
    }

    TSR::FrameBuffer* takeFirst()
    {
        QMutexLocker locker(&lock);
        TSR::FrameBuffer* buffer = vector.front();
        vector.pop_front();
        return buffer;
    }

    TSR::FrameBuffer* takeOne(qint32 frameNo)
    {
        QMutexLocker locker(&lock);
        TSR::FrameBuffer* buffer = nullptr;
        for(int v = 0; v < vector.count(); v++)
        {
            if(vector[v]->frameNumber() == frameNo)
            {
                buffer = vector[v];
                vector.remove(v);
                locker.unlock();
                VectorIsNotFull.wakeAll();
                return buffer;
            }
        }
        return buffer;
    }

    // 当前编码后的buffer 在 vector 中的数目，主要控制编码线程的同步
    qint32 threadBufferCount(qint32 threadId)
    {
        QMutexLocker locker(&lock);
        qint32 bufferCount = 0;
        QVectorIterator<TSR::FrameBuffer*> iter(vector);
        while(iter.hasNext())
        {
            if(iter.next()->encoderId() == threadId)
                bufferCount++;
        }
        return bufferCount;
    }

private:
    QVector<TSR::FrameBuffer*> vector;
    mutable QMutex lock;
    QWaitCondition VectorIsNotFull; //生产者可以往里面写数据
    QWaitCondition VectorIsNotEmpty;//消费者可以取数据，生产者写进数据后，就会释放QueueIsNotFull信号;
};

typedef ThreadSafeVector EncodedFramesVector;

#endif // THREADSAFEQUEUE_H
