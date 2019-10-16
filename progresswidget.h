#ifndef PROGRESSWIDGET_H
#define PROGRESSWIDGET_H

#include <QWidget>

namespace Ui {
class ProgressWidget;
}

class ProgressWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProgressWidget(QWidget *parent = nullptr);
    ~ProgressWidget();

private:
    Ui::ProgressWidget *ui;
};

#endif // PROGRESSWIDGET_H
