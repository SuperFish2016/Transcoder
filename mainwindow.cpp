#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "transcoder/transcoder.h"
#include <QCloseEvent>
#include <QFileDialog>
#include <QDir>
#include <QSettings>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QSettings s;
    s.beginGroup("TRANSCODE");
    ui->lineEdit->setText(s.value("VIDEO").toString());
    transcoder = nullptr;
    progressDlg = new ProgressWidget(ui->widget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(transcoder && transcoder->isTranscoding())
    {
        transcoder->stop();
    }
    event->accept();
}

void MainWindow::showImage(const QImage& image)
{
    ui->label->setPixmap(QPixmap::fromImage(image).scaled(ui->label->width(), ui->label->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::on_pushButton_clicked()
{
    if(transcoder != nullptr)
    {
        transcoder->stop();
        transcoder = nullptr;
    }
    else {
        VideoSource r;
        r.fileName = ui->lineEdit->text();
        r.frameCount = 30000;
        r.entryPoint = 0;
        r.duration = 29900;
        QList<VideoSource> list = QList<VideoSource>() << r;

        TranscoderOption params;
        params.sourceInfo.filename = r.fileName.toStdString();
        params.sourceInfo.startFrame = 0;
        params.sourceInfo.endFrame = 500;
        params.encodingThreadCount = 5;
        params.outputFilename = "ThisIsTestOutputFileName.mxf";
        transcoder = new Transcoder(params);
        connect(transcoder, &Transcoder::imageReady, this, &MainWindow::showImage);
        connect(transcoder, &Transcoder::currentProgress, progressDlg, &ProgressWidget::setProgress);
        connect(transcoder, &Transcoder::currentProgressText, progressDlg, &ProgressWidget::setProgressText);
        transcoder->start();
    }
}


void MainWindow::on_pushButton_3_clicked()
{
    QFile style_file("D:\\qss\\style.qss");
    style_file.open(QFile::ReadOnly | QIODevice::Text);
    qApp->setStyleSheet(QString::fromUtf8(style_file.readAll()));
}


void MainWindow::on_pushButton_4_clicked()
{
    QSettings s;
    s.beginGroup("TRANSCODE");
    QString filename = QFileDialog::getOpenFileName(this, "xx", s.value("VIDEO").toString());
    if(filename.isEmpty()) return;
    ui->lineEdit->setText(filename);
    s.setValue("VIDEO", ui->lineEdit->text());
}
