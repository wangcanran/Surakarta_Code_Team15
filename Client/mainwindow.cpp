#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include "pvpgame.h"
#include "ui_pvpgame.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow(int m)
{
    if(m==1)
    {

    }
    else if(m==2)
    {

    }
    else if(m==3)
    {

    }
}






void MainWindow::on_pushButton_clicked()
{
    PVPgame *game1=new PVPgame;
    game1->show();
    this->close();
}

