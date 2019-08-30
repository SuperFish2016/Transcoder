#ifndef WRITERTHREAD_H
#define WRITERTHREAD_H
#include <QThread>
#include <QTime>
#include "../transcoder.h"
#include "threadsafevector.h"
class Writer;
class WriteThread : public QThread
{
    Q_OBJECT
public:
    WriteThread(qint32 frameCount, EncodedFramesVector* j2kFrameVector, const QString& filename, volatile bool *stopped):
        frameCount_(frameCount),
        fileName_(filename),
        j2kFrameVector_(j2kFrameVector),
        stopped_(stopped)
    {

    }
protected:
    void run();
signals:
    void progress(int i);
    void progressText(const QString&);
    void reportStatus(TranscoderError stauts);
    void qImageReady(const QImage& image);
private:
    bool createWriter();
    void updateProgressBar(int currentFrame, int elapsedTime);
private:
    QTime  timer_;
    Writer* writer_;
    qint32  frameCount_;
    QString fileName_;
    EncodedFramesVector* j2kFrameVector_;
    volatile bool *stopped_;
};

#endif // WRITER_H
