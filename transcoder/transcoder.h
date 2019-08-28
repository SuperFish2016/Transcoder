#ifndef TRANSCODER_H
#define TRANSCODER_H

#include "utils/buffer.h"
#include "utils/videodefs.h"
#include "threads/threadsafequeue.h"
#include "threads/threadsafevector.h"

#include <QPointer>
#include <QProgressBar>
#include <QProgressDialog>
class WriteThread;
class EncodeThread;
class ReadThread;

enum enTranscodeError
{
    DecodeStart,
    DecodeEnd,
    DecodeError,
    EncodeError,
    EnocdeStart,
    EncodeEnd,
    WriteStart,
    WriteEnd,
    WriteError,
    WriterOpenError,
    WriterCloseError,
    UserCancel,
    NoError
};

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
    void setProgressBarVisible(bool visible);
    bool isTranscoding();
    void reportTranscodeResult();
    void start();
    void stop();
private:
    void createThreads();
    void createProgressBar();
    void cleanUp();

    bool okToCancel();
    QString getErrorString(enTranscodeError);
signals:
    void imageReady(const QImage& image);
private slots:
    void setProgress(int i);
    void setProgressText(const QString&);
    void cancelProgressBar();
public slots:
    void handleThreadStatus(enTranscodeError);
private:
    WriteThread*  writer_;
    ReadThread*   reader_;
    QList<QPointer<EncodeThread> >encoders_;

    DecodedFramesQueue  DecodedFramesQueue;
    EncodedFramesVector j2kFramesVector;

    volatile bool stopped_;

    QProgressDialog  progressBar_;
    enTranscodeError error_;
    TrancoderParams  transParams_;
    qint32 framesNeedBeWritten;
};
#endif // TRANSCODER_H
