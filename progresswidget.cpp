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
