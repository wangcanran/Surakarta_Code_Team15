#include "mainwindow.h"
#include "choose.h"
#include <QApplication>
#include <QTime>
#include "client.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

     QStringList args = QCoreApplication::arguments();
    /*
    Choose choose;
    choose.show();*/
     client Client(args);
    Client.show();


    return a.exec();
}
