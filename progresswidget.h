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

public slots:
    void setProgress(int i);
    void setProgressText(const QString& elapsedTime, const QString& remainingTime);
private:
    Ui::ProgressWidget *ui;
};

#endif // PROGRESSWIDGET_H
