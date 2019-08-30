#include "progressdialog.h"

ProgressDialog::ProgressDialog(QWidget* parent)
    :QProgressDialog(parent)
{

}

void ProgressDialog::createProgressBar()
{
//    progressBar_.setWindowModality(Qt::ApplicationModal);
//    progressBar_.setWindowFlags( Qt::Window  | Qt::WindowMinimizeButtonHint);
//    progressBar_.setRange(0, 100);
//    progressBar_.setValue(0);
//    progressBar_.setWindowTitle("Transcoding...");
//    progressBar_.setFixedWidth(600);
//#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
//    connect(&progressBar_, &QProgressDialog::canceled, this, &Transcoder::cancelProgressBar);
//#else
//    connect(&progressBar_, SIGNAL(canceled()), this, SLOT(cancelProgressBar()));
//#endif
}
