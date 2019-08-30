#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QProgressDialog>
#include <QProgressBar>
class ProgressDialog : public QProgressDialog
{
public:
    ProgressDialog(QWidget* parent = nullptr);
    void createProgressBar();
private:
    QProgressBar progressBar_;
};

#endif // PROGRESSDIALOG_H
