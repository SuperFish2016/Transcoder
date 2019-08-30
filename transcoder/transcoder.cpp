#include "transcoder.h"
#include "threads/encodethread.h"
#include "threads/readthread.h"
#include "threads/writethread.h"
#include <QMessageBox>
#include <QStyle>
#include <QApplication>
#include <QMetaType>

Transcoder::Transcoder(const TrancoderParams& transParams):
    stopped_(false), error_(TSR_SUCCESS), transParams_(transParams)
{
    qRegisterMetaType<TranscoderError>("TranscoderError");
    framesNeedBeWritten = 0;
    QListIterator<VideoSource> it(transParams_.videoList);
    while(it.hasNext())
    {
        framesNeedBeWritten += it.next().duration;
    }
    createThreads();
}

Transcoder::~Transcoder()
{
    qDebug() << "~Transcoder() called.";
}

void Transcoder::createThreads()
{
    reader_ = new ReadThread(&decodedFramesQueue, transParams_.videoList, &stopped_);
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    connect(reader_, &ReadThread::reportStatus, this, &Transcoder::handleThreadStatus);
#else
    connect(reader_, SIGNAL(reportStatus(enTranscodeError)), this, SLOT(handleThreadStatus(enTranscodeError)));
#endif
    writer_  = new WriteThread(framesNeedBeWritten, &encodedFramesVector, transParams_.outputFileName, &stopped_);
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    connect(writer_, &WriteThread::qImageReady, this, &Transcoder::imageReady);
    connect(writer_, &WriteThread::reportStatus, this, &Transcoder::handleThreadStatus);
    connect(writer_, &WriteThread::progress, this, &Transcoder::setProgress);
    connect(writer_, &WriteThread::progressText, this, &Transcoder::setProgressText);
#else
    connect(writer_, SIGNAL(reportStatus(enTranscodeError)), this, SLOT(handleThreadStatus(enTranscodeError)));
    connect(writer_, SIGNAL(progress(int)), this, SLOT(setProgress(int)));
    connect(writer_, SIGNAL(progressText(QString)), this, SLOT(setProgressText(QString)));
#endif
    const int maxBufferSize = J2KFramesVectorSize / transParams_.encodeThreadNumber;
    for(int i = 1; i <= transParams_.encodeThreadNumber; i++)
    {
        QPointer<EncodeThread> encoder = QPointer<EncodeThread>(new EncodeThread(&decodedFramesQueue, &encodedFramesVector, maxBufferSize, &stopped_));
#if(QT_VERSION >= QT_VERSION_CHECK(5,0,0))
        connect(encoder.data(), &EncodeThread::reportStatus, this, &Transcoder::handleThreadStatus);
#else
        connect(encoder.data(), SIGNAL(reportStatus(enTranscodeError)), this, SLOT(handleThreadStatus(enTranscodeError)));
#endif
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

bool Transcoder::okToCancel()
{
//    QScopedPointer<QMessageBox> messageBox(new QMessageBox(nullptr));
//    messageBox->setWindowFlags( Qt::Dialog | Qt::WindowCloseButtonHint );
//    messageBox->setWindowModality(Qt::WindowModal);
//    messageBox->setIcon(QMessageBox::Question);
//    messageBox->setWindowTitle("Cancel Transcoding");
//    messageBox->setText("Are you sure you want to interrupt the transcoding operation?");

//    messageBox->addButton(QMessageBox::Yes);
//    messageBox->addButton(QMessageBox::No);
//    messageBox->exec();
//    if(messageBox->clickedButton() == messageBox->button(QMessageBox::Yes))
//        return true;
//    return false;
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
