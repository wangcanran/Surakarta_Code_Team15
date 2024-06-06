#include "mainwindow.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //使用命令行启动
    QStringList args = QCoreApplication::arguments();
    MainWindow w(args);
    w.show();

return a.exec();
}
