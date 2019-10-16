#ifndef ENCODERTHREAD_H
#define ENCODERTHREAD_H

#include <QThread>
#include "../transcoder.h"
class Encoder;
class EncodeThread : public QThread
{
    Q_OBJECT
public:
    EncodeThread(DecodedFramesQueue* DecodedFramesQueue, EncodedFramesVector* encodedFramesVector, qint32 maxBufferSize, volatile bool *stopped, TranscoderOption* transOption);
    ~EncodeThread(){}
    void setId(qint32 id){id_ = id;}
    qint32 id() const{ return id_;}
    TSR::FrameBuffer* takeOneFrame();
signals:
    void reportStatus(TranscoderError stauts);
protected:
    void run();
private:
    DecodedFramesQueue*  DecodedFramesQueue_;
    EncodedFramesVector* encodedFramesVector_;
    qint32 maxBufferSize_;
    qint32 id_;
    Encoder* encoder_;
    volatile bool *stopped_;
    TranscoderOption* transOptions_;
};

#endif // ENCODERTHREAD_H
