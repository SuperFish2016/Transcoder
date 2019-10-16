#include "transcoder.h"
#include "threads/encodethread.h"
#include "threads/readthread.h"
#include "threads/writethread.h"
#include <QMessageBox>
#include <QStyle>
#include <QApplication>
#include <QMetaType>

Transcoder::Transcoder(const TranscoderOption& transParams):
    stopped_(false), error_(TSR_SUCCESS), transOptions_(transParams)
{
    qRegisterMetaType<TranscoderError>("TranscoderError");
    framesNeedBeWritten = transOptions_.sourceInfo.endFrame - transOptions_.sourceInfo.startFrame + 1;
    createThreads();
}

Transcoder::~Transcoder()
{
    qDebug() << "~Transcoder() called.";
}

void Transcoder::createThreads()
{
    reader_ = new ReadThread(&decodedFramesQueue, &stopped_, &transOptions_);
    connect(reader_, SIGNAL(reportStatus(TranscoderError)), this, SLOT(handleThreadStatus(TranscoderError)));
    writer_  = new WriteThread(framesNeedBeWritten, &encodedFramesVector, QString::fromStdString(transOptions_.outputFilename), &stopped_);

    connect(writer_, SIGNAL(reportStatus(TranscoderError)), this, SLOT(handleThreadStatus(TranscoderError)));
    connect(writer_, SIGNAL(progress(int)),                 this, SIGNAL(currentProgress(int)));
    connect(writer_, SIGNAL(progressText(const QString&, const QString& )),         this, SIGNAL(currentProgressText(const QString&, const QString& )));
    connect(writer_, SIGNAL(qImageReady(const QImage&)),   this, SIGNAL(imageReady(const QImage&)));

    const int maxBufferSize = EncodedFramesBufferSize / transOptions_.encodingThreadCount;
    for(int i = 1; i <= transOptions_.encodingThreadCount; i++)
    {
        QPointer<EncodeThread> encoder = QPointer<EncodeThread>(new EncodeThread(&decodedFramesQueue, &encodedFramesVector, maxBufferSize, &stopped_, &transOptions_));
        connect(encoder.data(), SIGNAL(reportStatus(TranscoderError)), this, SLOT(handleThreadStatus(TranscoderError)));
        encoder.data()->setId(i);
        encoders_.push_back(encoder);
    }
}

void Transcoder::stop()
{
    stopped_ = true;
    decodedFramesQueue.enqueueFinished();
    while(this->isTranscoding());
    cleanUp();
}

void Transcoder::start()
{
    reader_->start();
    writer_->start();
    QListIterator<QPointer<EncodeThread> >iter(encoders_);
    while(iter.hasNext())
    {
        iter.next().data()->start();
    }
    while(this->isTranscoding()){
        qApp->processEvents();
    }
}

QString Transcoder::getErrorString(TranscoderError e)
{
//    if(e == DecodeError) return "Frame deocde error.";
//    if(e == WriteError)  return "Frame write error.";
//    if(e == EncodeError) return "Frame encode error.";
//    if(e == NoError) return "No error.";
//    if(e == WriterOpenError) return "Writer open error.";
//    return "Transcode Successfully.";
}


void Transcoder::cleanUp()
{
    while(decodedFramesQueue.conut() != 0)
    {
        FrameBuffer* buffer = decodedFramesQueue.dequeue();
        delete buffer;
    }
    while(encodedFramesVector.conut() != 0)
    {
        FrameBuffer* buffer = encodedFramesVector.takeFirst();
        delete buffer;
    }
}


bool Transcoder::isTranscoding()
{
    if(reader_->isRunning() || writer_->isRunning()) return true;
    QListIterator<QPointer<EncodeThread> >it(encoders_);
    while(it.hasNext())
    {
        if((it.next()).data()->isRunning())
            return true;
    }
    return false;
}

void Transcoder::handleThreadStatus(TranscoderError error)
{
    switch (error) {
    case TSR_DECODER_END:
        decodedFramesQueue.enqueueFinished(); // avoid encode thread can not end normally.
        break;
    case TSR_DECODER_ERROR:
    case TSR_WRITER_ERROR:
    case TSR_ENCODER_ERROR:
    case TSR_USER_CANCEL:
    case TSR_WRITER_OPEN_ERROR:
        error_ = error;
        stopped_ = true; // stop all thread.
        decodedFramesQueue.enqueueFinished(); // avoid encode thread waite.
        break;
    default:
        break;
    }
}

void Transcoder::cancelTranscoding()
{
    handleThreadStatus(TSR_USER_CANCEL);
    stop();
}
