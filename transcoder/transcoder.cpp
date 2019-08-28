#include "transcoder.h"
#include "threads/encodethread.h"
#include "threads/readthread.h"
#include "threads/writethread.h"
#include <QMessageBox>
#include <QStyle>
#include <QApplication>
#include <QMetaType>

Transcoder::Transcoder(const TrancoderParams& transParams):
    stopped_(false), error_(NoError), transParams_(transParams)
{
    qRegisterMetaType<enTranscodeError>("enTranscodeError");

    framesNeedBeWritten = 0;
    QListIterator<VideoSource> it(transParams_.videoList);
    while(it.hasNext())
    {
        framesNeedBeWritten += it.next().duration;
    }
    createThreads();
    createProgressBar();
}

Transcoder::~Transcoder()
{
    qDebug() << "~Transcoder() called.";
}

void Transcoder::createThreads()
{
    reader_ = new ReadThread(&DecodedFramesQueue, transParams_.videoList, &stopped_);
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    connect(reader_, &ReadThread::reportStatus, this, &Transcoder::handleThreadStatus);
#else
    connect(reader_, SIGNAL(reportStatus(enTranscodeError)), this, SLOT(handleThreadStatus(enTranscodeError)));
#endif
    writer_  = new WriteThread(framesNeedBeWritten, &j2kFramesVector, transParams_.outputFileName, &stopped_);
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
        QPointer<EncodeThread> encoder = QPointer<EncodeThread>(new EncodeThread(&DecodedFramesQueue, &j2kFramesVector, maxBufferSize, &stopped_));
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
    DecodedFramesQueue.enqueueFinished();
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
    QScopedPointer<QMessageBox> messageBox(new QMessageBox(&progressBar_));
    messageBox->setWindowFlags( Qt::Dialog | Qt::WindowCloseButtonHint );
    messageBox->setWindowModality(Qt::WindowModal);
    messageBox->setIcon(QMessageBox::Question);
    messageBox->setWindowTitle("Cancel Transcoding");
    messageBox->setText("Are you sure you want to interrupt the transcoding operation?");

    messageBox->addButton(QMessageBox::Yes);
    messageBox->addButton(QMessageBox::No);
    messageBox->exec();
    if(messageBox->clickedButton() == messageBox->button(QMessageBox::Yes))
        return true;
    return false;
}

QString Transcoder::getErrorString(enTranscodeError e)
{
    if(e == DecodeError) return "Frame deocde error.";
    if(e == WriteError)  return "Frame write error.";
    if(e == EncodeError) return "Frame encode error.";
    if(e == NoError) return "No error.";
    if(e == WriterOpenError) return "Writer open error.";
    return "Transcode Successfully.";
}

void Transcoder::reportTranscodeResult()
{
    if(error_ == UserCancel) return; // user cancel will not report
    QScopedPointer<QMessageBox> messageBox(new QMessageBox());
    messageBox->setWindowModality(Qt::WindowModal);
    messageBox->setWindowFlags( Qt::Dialog | Qt::WindowCloseButtonHint );
    QMessageBox::Icon icon = error_ != NoError ? QMessageBox::Critical : QMessageBox::Information;

    QString errorMessage = getErrorString(error_);
    messageBox->setIcon(icon);
    if(error_ != NoError)
    {
        messageBox->setWindowTitle("Trancoded Failed");
        messageBox->setText(errorMessage);
    }
    else
    {
        messageBox->setWindowTitle("Trancode Successfully");
        messageBox->setText(errorMessage);
    }
    messageBox->addButton(QMessageBox::Ok);
    messageBox->exec();
}

void Transcoder::cleanUp()
{
    while(DecodedFramesQueue.conut() != 0)
    {
        FrameBuffer* buffer = DecodedFramesQueue.dequeue();
        delete buffer;
    }
    while(j2kFramesVector.conut() != 0)
    {
        FrameBuffer* buffer = j2kFramesVector.takeFirst();
        delete buffer;
    }
}

void Transcoder::setProgressBarVisible(bool visible)
{
    if(visible)
        progressBar_.show();
    else
        progressBar_.hide();
}

void Transcoder::createProgressBar()
{   
    progressBar_.setWindowModality(Qt::ApplicationModal);
    progressBar_.setWindowFlags( Qt::Window  | Qt::WindowMinimizeButtonHint);
    progressBar_.setRange(0, framesNeedBeWritten);
    progressBar_.setValue(0);
    progressBar_.setWindowTitle("Transcoding...");
    progressBar_.setWindowIcon(progressBar_.style()->standardIcon(QStyle::SP_MediaPlay));
    progressBar_.setFixedWidth(600);
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    connect(&progressBar_, &QProgressDialog::canceled, this, &Transcoder::cancelProgressBar);
#else
    connect(&progressBar_, SIGNAL(canceled()), this, SLOT(cancelProgressBar()));
#endif
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

void Transcoder::setProgress(int i)
{
    progressBar_.setValue(i);
    progressBar_.update(); //Mac OS need update to refect the slide progress.
}

void Transcoder::setProgressText(const QString &text)
{
    progressBar_.setLabelText(text);
}

void Transcoder::handleThreadStatus(enTranscodeError error)
{
    switch (error) {
    case DecodeEnd:
        DecodedFramesQueue.enqueueFinished(); // avoid encode thread can not end normally.
        break;
    case DecodeError:
    case WriteError:
    case EncodeError:
    case UserCancel:
    case WriterOpenError:
        error_ = error;
        stopped_ = true; // stop all thread.
        DecodedFramesQueue.enqueueFinished(); // avoid encode thread waite.
        progressBar_.hide(); // hide progress bar.
        break;
    default:
        break;
    }
}

void Transcoder::cancelProgressBar()
{
    if(okToCancel())
    {
        handleThreadStatus(UserCancel);
        stop();
    }
}
