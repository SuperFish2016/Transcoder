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
    WriteThread(qint32 frameCount, EncodedFramesVector* encodedFramesVector, const QString& filename, volatile bool *stopped):
        frameCount_(frameCount),
        fileName_(filename),
        encodedFramesVector_(encodedFramesVector),
        stopped_(stopped)
    {

    }
protected:
    void run();
signals:
    void progress(int i);
    void progressText(const QString& elapsed, const QString& remainint);
    void reportStatus(TranscoderError stauts);
    void qImageReady(const QImage& image);
private:
    bool createWriter();
    void updateProgressText(int currentFrame, int elapsedTime);
private:
    QTime  timer_;
    Writer* writer_;
    qint32  frameCount_;
    QString fileName_;
    EncodedFramesVector* encodedFramesVector_;
    volatile bool *stopped_;
};

#endif // WRITER_H
