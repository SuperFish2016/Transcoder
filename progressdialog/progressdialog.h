#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QProgressDialog>

class ProgressDialog : public QProgressDialog
{
public:
    ProgressDialog(QWidget* parent = nullptr);
};

#endif // PROGRESSDIALOG_H
