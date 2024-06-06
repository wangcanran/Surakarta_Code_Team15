#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <chrono>
#include <iostream>
#include <thread>
#include "surakarta_common.h"
#include "surakarta_game.h"
#include <QRegularExpression>
#include <fstream>

#define ANSI_CLEAR_SCREEN "\033[2J"
#define ANSI_MOVE_TO_START "\033[H"

MainWindow::MainWindow(const QStringList &args, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    port = 10086;//默认端口
    server = new NetworkServer(this); // 创建一个服务器对象，其父对象为当前窗口

    ui->ShowClient1->setReadOnly(true);
    ui->ShowClient2->setReadOnly(true);
    //计时器
    timer=new QTimer(this);
    timer->start(1400);//1.4s调用一次计时器函数
    //默认超时时间为3s
    minute=0;
    second=3;
    m_=minute;
    s_=second;
    //定向输出到该文件
    file.setFileName("./record/server/Team_15.txt");
    //每次打开都会覆盖前一次的内容
    file.open(QIODevice::WriteOnly|QIODevice::Text);

     //默认开启监听
    server->listen(QHostAddress::Any, this->port);
    // 一行代码搞定监听，它会在所有 ip 地址上监听指定端口 port
    // 所谓监听，就是在网络上服务器要开始盯住某个端口，此时客户端可以通过这个端口和服务器取得联系
    // QHostAddress::Any 表示监听所有 ip 地址，在不考虑安全的情况下，这是一个比较方便的选择，你不用纠结为什么
    ui->PortButton->setEnabled(false);
    ui->PortEdit->setReadOnly(true);
    ui->PortEdit->setText("Listening...");

    //connect(ui->PortButton, &QPushButton::clicked, this, &MainWindow::listen_port); // 开启服务器监听
    connect(ui->restart, &QPushButton::clicked, this, &MainWindow::restart_server); // 重启服务器
    connect(server, &NetworkServer::receive, this, &MainWindow::receive_from_client);
    // receive 是服务端收到消息后发出的信号，receive_from_client 是处理这个信号的槽函数
}

void MainWindow::listen_port()
{
    this->port = ui->PortEdit->text().toInt();
    server->listen(QHostAddress::Any, this->port);
    // 一行代码搞定监听，它会在所有 ip 地址上监听指定端口 port
    // 所谓监听，就是在网络上服务器要开始盯住某个端口，此时客户端可以通过这个端口和服务器取得联系
    // QHostAddress::Any 表示监听所有 ip 地址，在不考虑安全的情况下，这是一个比较方便的选择，你不用纠结为什么
    ui->PortButton->setEnabled(false);
    ui->PortEdit->setReadOnly(true);
    ui->PortEdit->setText("Listening...");
}

void MainWindow::remove_client(QTcpSocket *client)
{
    if (client == client1)
    {
        client1 = nullptr;
        ui->ShowClient1->setText("");
    }
    else if (client == client2)
    {
        client2 = nullptr;
        ui->ShowClient2->setText("");
    }
    clients.remove(client);
}

void MainWindow::receive_from_client(QTcpSocket *client, NetworkData data)
{
    // client 是发送消息的客户端，data 是消息内容
    // 以下代码首先处理客户端的连接和断开，然后处理从客户端收到的消息，显示在界面上并转发给另一个客户端
    // 你们在游戏里要做类似的事情，只不过处理不仅仅是显示，而是对应的游戏逻辑
    QTextStream in(&file);
    if (!clients.contains(client))
    {
        if (clients.size() >= max_clients)
        {
            QMessageBox::warning(this, "Warning", "The server is full!");

            return;
        }
        clients.insert(client);
        if (!client1)
        {
            client1 = client;
        }
        else if (!client2)
        {
            client2 = client;
        }
    }
    //处理READY_OP信号
    if (data.op == OPCODE::READY_OP)
    {
        //若b==0则说明BLACK没有被分配给任何客户
        if (data.data2 == QString::fromStdString(std::to_string(static_cast<int>(PieceColor::BLACK))) && b == 0)
        {

            name[0] = data.data1;
            QRegularExpression re("^[a-zA-Z0-9_]+$");
            // 使用QRegularExpression的match方法来进行匹配，判断用户名合法性
            QRegularExpressionMatch match = re.match(name[0]);
            if (match.hasMatch())
            {

                client1 = client;
                b = 1;
            }
            else
            {
                send_to_another_client(client, NetworkData(OPCODE::REJECT_OP, name[0], "", ""));
            }
        }
        //若w==0则说明WHITE还没有被分配给任何用户
        else if (w == 0 && data.data2 == QString::fromStdString(std::to_string(static_cast<int>(PieceColor::WHITE))))
        {
            name[1] = data.data1;
            QRegularExpression re("^[a-zA-Z0-9_]+$");
            // 使用QRegularExpression的match方法来进行匹配，判断用户名合法性
            QRegularExpressionMatch match = re.match(name[1]);
            if (match.hasMatch())
            {
                client2 = client;
                w = 1;
            }
            else
                send_to_another_client(client, NetworkData(OPCODE::REJECT_OP, name[1], "", ""));//判断不符合要求后发送信号给另一方
        }
        //若b==1则说明BLACK已经被分配给其他客户
        else if (b == 1 && data.data2 == QString::fromStdString(std::to_string(static_cast<int>(PieceColor::BLACK))))
        {
            QString s = data.data1;
            send_to_another_client(client, NetworkData(OPCODE::REJECT_OP, s, "", ""));
        }
        //同上
        else if (w == 1 && data.data2 == QString::fromStdString(std::to_string(static_cast<int>(PieceColor::WHITE))))
        {
            QString s = data.data1;
            send_to_another_client(client, NetworkData(OPCODE::REJECT_OP, s, "", ""));
        }
    }
    //双方各选择了一种颜色后，给双方发送READY_OP表示游戏开始
    if (data.op == OPCODE::READY_OP && b == 1 && w == 1)
    {
        NetworkData d(OPCODE::READY_OP, name[0], QString::fromStdString(std::to_string(static_cast<int>(PieceColor::BLACK))), QString::number(1));
        send_to_another_client(client2, NetworkData(OPCODE::READY_OP, name[0], QString::fromStdString(std::to_string(static_cast<int>(PieceColor::WHITE))), QString::number(1)));
        send_to_another_client(client1, NetworkData(OPCODE::READY_OP, name[1], QString::fromStdString(std::to_string(static_cast<int>(PieceColor::BLACK))), QString::number(1)));
        game.StartGame();
    }
    //收到client1发送的信号
    if (client == client1)
    {
        //显示聊天框
        this->ui->ShowClient1->setText(data.data2);
        if (client2 && data.op == OPCODE::CHAT_OP)
            send_to_another_client(client2, data);
        //处理move的信息
        if (client2 && data.op == OPCODE::MOVE_OP)
        {
            send_to_another_client(client2, data);
            //计时器
            s_=second;
            m_=minute;
            connect(timer,SIGNAL(timeout()),this,SLOT(Clock2()));
            disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock1()));
            //将data中的数据转换成move，传入game中，以保证同步性
            QString s1 = data.data1, s2 = data.data2, s3;
            SurakartaMove move=change(s1, s2, PieceColor::BLACK);
            game.Move(move);
            SurakartaRuleManager *manager=new SurakartaRuleManager(game.board_,game.game_info_);
            SurakartaIllegalMoveReason reason = manager->JudgeMove(move);
            //判断游戏是否结束
            if (game.IsEnd())
            {
                s1 = QString::fromStdString(std::to_string(static_cast<int>(reason)));
                s2 = QString::fromStdString(std::to_string(static_cast<int>(game.GetGameInfo()->end_reason_)));
                s3 = QString::fromStdString(std::to_string(static_cast<int>(game.game_info_->winner_)));
                send_to_another_client(client2, NetworkData(OPCODE::END_OP, s1, s2, s3));
                send_to_another_client(client1, NetworkData(OPCODE::END_OP, s1, s2, s3));
                //输出结束原因
                if(static_cast<int>(game.GetGameInfo()->end_reason_)==1)in<<'S'<<'#';
                if(static_cast<int>(game.GetGameInfo()->end_reason_)==2)in<<'C'<<'#';
                if(static_cast<int>(game.GetGameInfo()->end_reason_)==4)in<<'R'<<'#';
                if(static_cast<int>(game.GetGameInfo()->end_reason_)==5)in<<'T'<<'#';
                if(static_cast<int>(game.GetGameInfo()->end_reason_)==6)in<<'I'<<'#';
                //关闭倒计时
                disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock1()));
                disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock2()));
          }
        }
        //处理投降信号，判断投降信号是不是在自己的回合发出
        if (client2 && data.op == OPCODE::RESIGN_OP&& game.GetGameInfo()->current_player_==PieceColor::BLACK)
        {
            QString s1 = data.data1, s2 = data.data2, s3;
            s3 = QString::fromStdString(std::to_string(static_cast<int>(PieceColor::WHITE)));
            send_to_another_client(client2, NetworkData(OPCODE::END_OP, "", QString::number((int)SurakartaEndReason::RESIGN), s3));
            send_to_another_client(client1, NetworkData(OPCODE::END_OP, "", QString::number((int)SurakartaEndReason::RESIGN), s3));
            in<<'R'<<'#';
        }
        //如果在游戏时选择离开，则视为投降
        if (client2 && data.op == OPCODE::LEAVE_OP)
        {
            if ((!game.IsEnd()))
            {
                in<<'R'<<'#';
                send_to_another_client(client2, NetworkData(OPCODE::END_OP, "", QString::fromStdString(std::to_string(static_cast<int>(SurakartaEndReason::RESIGN))), QString::fromStdString(std::to_string(static_cast<int>(PieceColor::WHITE)))));
                send_to_another_client(client1, NetworkData(OPCODE::END_OP, "", QString::fromStdString(std::to_string(static_cast<int>(SurakartaEndReason::RESIGN))), QString::fromStdString(std::to_string(static_cast<int>(PieceColor::WHITE)))));
            }
            remove_client(client);
            return;
        }
    }
    else if (client == client2)
    {
        //聊天功能
        this->ui->ShowClient2->setText(data.data2);
        if (client1 && data.op == OPCODE::CHAT_OP)
            send_to_another_client(client1, data);
        //处理move信号
        if (client1 && data.op == OPCODE::MOVE_OP)
        {
            send_to_another_client(client1, data);
            //计时器
            QString ss1,ss2,ss3;
            s_=second;
            m_=minute;
            connect(timer,SIGNAL(timeout()),this,SLOT(Clock1()));
            disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock2()));
            //保持Server内的游戏与客户端的游戏同步
            QString s1 = data.data1, s2 = data.data2, s3;
            SurakartaMove move=change(s1, s2, PieceColor::WHITE);
            SurakartaIllegalMoveReason reason = game.rule_manager_->JudgeMove(move);
            game.Move(move);
            //判断游戏结束
            if (game.IsEnd())
            {
                s1 = QString::fromStdString(std::to_string(static_cast<int>(reason)));
                s2 = QString::fromStdString(std::to_string(static_cast<int>(game.GetGameInfo()->end_reason_)));
                s3 = QString::fromStdString(std::to_string(static_cast<int>(game.game_info_->winner_)));
                disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock1()));
                disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock2()));
                send_to_another_client(client1, NetworkData(OPCODE::END_OP, s1, s2, s3));
                send_to_another_client(client2, NetworkData(OPCODE::END_OP, s1, s2, s3));

                if(static_cast<int>(game.GetGameInfo()->end_reason_)==1)in<<'S'<<'#';
                if(static_cast<int>(game.GetGameInfo()->end_reason_)==2)in<<'C'<<'#';
                if(static_cast<int>(game.GetGameInfo()->end_reason_)==4)in<<'R'<<'#';
                if(static_cast<int>(game.GetGameInfo()->end_reason_)==5)in<<'T'<<'#';
                if(static_cast<int>(game.GetGameInfo()->end_reason_)==6)in<<'I'<<'#';
            }
        }
        //处理投降信号，判断投降信号是不是在自己的回合发出
        if (client1 && data.op == OPCODE::RESIGN_OP&& game.GetGameInfo()->current_player_==PieceColor::WHITE)
        {
            QString s1 = data.data1, s2 = data.data2, s3;

            s3 = QString::fromStdString(std::to_string(static_cast<int>(PieceColor::BLACK)));
            send_to_another_client(client2, NetworkData(OPCODE::END_OP, "", QString::number((int)SurakartaEndReason::RESIGN), s3));
            send_to_another_client(client1, NetworkData(OPCODE::END_OP, "", QString::number((int)SurakartaEndReason::RESIGN), s3));
            file.open(QIODevice::Append|QIODevice::Text);
            in<<'R'<<'#';
        }
        //游戏未结束就退出，视为投降
        if (client2 && data.op == OPCODE::LEAVE_OP)
        {
            if ((!game.IsEnd()))
            {
                file.open(QIODevice::Append|QIODevice::Text);
                in<<'R'<<'#';
                send_to_another_client(client1, NetworkData(OPCODE::END_OP, "", QString::fromStdString(std::to_string(static_cast<int>(SurakartaEndReason::RESIGN))), QString::fromStdString(std::to_string(static_cast<int>(PieceColor::BLACK)))));
                send_to_another_client(client2, NetworkData(OPCODE::END_OP, "", QString::fromStdString(std::to_string(static_cast<int>(SurakartaEndReason::RESIGN))), QString::fromStdString(std::to_string(static_cast<int>(PieceColor::BLACK)))));
            }
            remove_client(client);
            return;
        }
    }

    else
        QMessageBox::warning(this, "Warning", "Unknown client!");
}

void MainWindow::send_to_another_client(QTcpSocket *another, NetworkData data)
{
    // 发送消息给一个客户端，这个程序里实际上做的事情是转发消息
    this->server->send(another, data);
}

void MainWindow::restart_server()
{
    // 如果你要重启服务器，就像其他 QObject 一样，你需要 disconnect 再 connect 回来一些信号槽
    server->close();
    clients.clear();
    client1 = nullptr;
    client2 = nullptr;
    disconnect(server, &NetworkServer::receive, this, &MainWindow::receive_from_client);
    delete server;
    server = new NetworkServer(this);
    ui->PortButton->setEnabled(true);
    ui->PortEdit->setReadOnly(false);
    ui->PortEdit->setText(QString::number(this->port));
    ui->ShowClient1->setText("");
    ui->ShowClient2->setText("");
    connect(server, &NetworkServer::receive, this, &MainWindow::receive_from_client);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete server;
}

void MainWindow::Clock1()
{
    //处理分、秒转换
    if((m_!=0)||(s_!=0)){
        if(s_==0){
            s_=59;
            m_--;}
        else
            s_--;
    }
    //如果倒计时结束，则向双方发送游戏结束的消息
     QTextStream in(&file);
    if(m_==0&&s_==0)
    {
         QString ss1,ss2;
        game.game_info_->winner_=PieceColor::WHITE;
        game.game_info_->end_reason_=SurakartaEndReason::TIMEOUT;
        disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock1()));
        disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock2()));
        ss1=QString::fromStdString(std::to_string(static_cast<int>(SurakartaEndReason::TIMEOUT)));
        ss2=QString::QString::fromStdString(std::to_string(static_cast<int>(PieceColor::WHITE)));

        send_to_another_client(client1,NetworkData(OPCODE::END_OP,"",ss1,ss2));
        send_to_another_client(client2,NetworkData(OPCODE::END_OP,"",ss1,ss2));

        in<<'T'<<'#';
        return;
    }


}

void MainWindow::Clock2()
{
    if((minute!=0)||(second!=0)){
        if(s_==0){
            s_=59;
            m_--;}
        else
            s_--;
    }
     QTextStream in(&file);
    if(m_==0&&s_==0)
    {
         QString ss1,ss2;
        game.game_info_->winner_=PieceColor::BLACK;
        game.game_info_->end_reason_=SurakartaEndReason::TIMEOUT;
        in<<'T'<<'#';
        disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock1()));
        disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock2()));
        ss1=QString::fromStdString(std::to_string(static_cast<int>(SurakartaEndReason::TIMEOUT)));
        ss2=QString::QString::fromStdString(std::to_string(static_cast<int>(PieceColor::BLACK)));

        send_to_another_client(client1,NetworkData(OPCODE::END_OP,"",ss1,ss2));
        send_to_another_client(client2,NetworkData(OPCODE::END_OP,"",ss1,ss2));
        return;
    }
}

SurakartaMove MainWindow::change(QString a,QString b,PieceColor color)
{
    int a1,a2,b1,b2;
    if(a[0]=='A')a1=0;
    if(a[0]=='B')a1=1;
    if(a[0]=='C')a1=2;
    if(a[0]=='D')a1=3;
    if(a[0]=='E')a1=4;
    if(a[0]=='F')a1=5;

    if(a[1]=='1')a2=0;
    if(a[1]=='2')a2=1;
    if(a[1]=='3')a2=2;
    if(a[1]=='4')a2=3;
    if(a[1]=='5')a2=4;
    if(a[1]=='6')a2=5;

    if(b[0]=='A')b1=0;
    if(b[0]=='B')b1=1;
    if(b[0]=='C')b1=2;
    if(b[0]=='D')b1=3;
    if(b[0]=='E')b1=4;
    if(b[0]=='F')b1=5;

    if(b[1]=='1')b2=0;
    if(b[1]=='2')b2=1;
    if(b[1]=='3')b2=2;
    if(b[1]=='4')b2=3;
    if(b[1]=='5')b2=4;
    if(b[1]=='6')b2=5;

    SurakartaPosition from(a1, a2);
    SurakartaPosition to(b1, b2);

    SurakartaMove move(from, to, color);
    QTextStream in(&file);
    in<<a<<'-'<<b<<' ';
    return move;
}


