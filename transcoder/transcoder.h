#ifndef TRANSCODER_H
#define TRANSCODER_H

#include "utils/buffer.h"
#include  "utils/transcoder_common.h"
#include "utils/videodefs.h"
#include "threads/threadsafequeue.h"
#include "threads/threadsafevector.h"
#include <QPointer>
#include <QProgressBar>
#include <QProgressDialog>
class WriteThread;
class EncodeThread;
class ReadThread;

class Transcoder : public QObject
{
    Q_OBJECT
public:
    struct TrancoderParams
    {
        QList<VideoSource> videoList;
        qint32  encodeThreadNumber;
        QString outputFileName;
    };

public:
    Transcoder(const TrancoderParams& transParams);
    ~Transcoder();
    /* return true is transcoder is running. */
    bool isTranscoding();
    /* run all working threads in transcoder*/
    void start();
    /* stop all working threads in transcoder*/
    void stop();
private:
    void createThreads();
    void cleanUp();
    bool okToCancel();
    QString getErrorString(TranscoderError);
signals:
    void imageReady(const QImage& image);
    /* current progress */
    void setProgress(int i);
    /* current progress text. eg: elapsed time: 00:10:00:00 */
    void setProgressText(const QString&);
public slots:
    void cancelTranscoding();
private slots:
    void handleThreadStatus(TranscoderError);
private:
    QPointer<WriteThread>  writer_;
    QPointer<ReadThread>   reader_;
    QList<QPointer<EncodeThread> >encoders_;
    DecodedFramesQueue  decodedFramesQueue;
    EncodedFramesVector encodedFramesVector;

    volatile bool stopped_;

    TranscoderError  error_;
    TrancoderParams  transParams_;
    qint32 framesNeedBeWritten;
};
#endif // TRANSCODER_H
