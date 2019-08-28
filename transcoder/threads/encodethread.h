#ifndef ENCODERTHREAD_H
#define ENCODERTHREAD_H

#include <QThread>
#include "../transcoder.h"
class Encoder;
class EncodeThread : public QThread
{
    Q_OBJECT
public:
    EncodeThread(DecodedFramesQueue* DecodedFramesQueue, EncodedFramesVector* j2kFramesVector, quint32 maxBufferSize, volatile bool *stopped);
    ~EncodeThread(){}
    void setId(qint32 id){id_ = id;}
    qint32 id() const{ return id_;}
    TSR::FrameBuffer* takeOneFrame();
signals:
    void reportStatus(enTranscodeError stauts);
protected:
    void run();
private:
    DecodedFramesQueue*  DecodedFramesQueue_;
    EncodedFramesVector* encodedFramesVector_;
    qint32 maxBufferSize_;
    qint32 id_;
    volatile bool *stopped_;
    Encoder* encoder_;
};

#endif // ENCODERTHREAD_H
