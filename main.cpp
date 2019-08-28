#include <QApplication>
#include "transcoder/transcoder.h"
#include <QDebug>
#include <QEventLoop>
#include "mainwindow.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    QFile style_file(":\style.qss");
    style_file.open(QFile::ReadOnly | QIODevice::Text);
    a.setStyleSheet(QString::fromUtf8(style_file.readAll()));
    return a.exec();
}
