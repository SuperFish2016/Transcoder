#include "progresswidget.h"
#include "ui_progresswidget.h"

ProgressWidget::ProgressWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProgressWidget)
{
    ui->setupUi(this);
}

ProgressWidget::~ProgressWidget()
{
    delete ui;
}

void ProgressWidget::setProgress(int i)
{
    ui->progressBar->setValue(i);
}

void ProgressWidget::setProgressText(const QString& elapsedTime, const QString& remainingTime)
{
    ui->ui_elapsed_time->setText(elapsedTime);
    ui->ui_remaning_time->setText(remainingTime);
}
