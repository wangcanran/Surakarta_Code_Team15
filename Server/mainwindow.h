#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSet>
#include <QMessageBox>
#include "networkdata.h"
#include "networkserver.h"
#include <chrono>
#include <iostream>
#include <thread>
#include "surakarta_common.h"
#include "surakarta_game.h"
#include <QTimer>
#include <fstream>
#include <QFile>
#include <QTextStream>
#include <QCommandLineParser>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QStringList &args,QWidget *parent = nullptr);
    ~MainWindow();
    SurakartaGame game;//在Server内开一个与Client同步的游戏
    QString name[2];
    //用来判断用户选择的棋子颜色情况
    int b=0;
    int w=0;
    //用于倒计时
    QTimer *timer;
    int minute,m_;
    int second,s_;

    QFile file;
    //将data中的信息换成move所需信息
    SurakartaMove change(QString a,QString b,PieceColor color);

private:
    int port;
    const int max_clients = 2;
    Ui::MainWindow *ui;
    QSet<QTcpSocket*> clients;
    NetworkServer* server = nullptr;
    QTcpSocket* client1 = nullptr;
    QTcpSocket* client2 = nullptr;
    void send_to_another_client(QTcpSocket* another, NetworkData data);
    void remove_client(QTcpSocket* client);

private slots:
    void listen_port();
    void restart_server();
    void receive_from_client(QTcpSocket* client, NetworkData data);
    void Clock1();
    void Clock2();
};
#endif // MAINWINDOW_H
