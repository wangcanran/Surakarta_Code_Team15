#include "client.h"
#include "./ui_client.h"
#include "networkdata.h"
#include "networkserver.h"
#include "surakarta_piece.h"
#include "surakarta_game.h"
#include "surakarta_board.h"
#include "surakarta_common.h"
#include "surakarta_agent_random.h"
#include "surakarta_rule_manager.h"
#include "iostream"
#include "aithread.h"
#include "choose.h"
#include <QString>
#include "chess.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <QDebug>
#include <QPixmap>
#include <QPainterPath>
#include <QLCDNumber>
#include <QString>
#include <QHBoxLayout>
#include <QTimer>
#include <QPropertyAnimation>
#include <QVariant>
#include <QPainterPath>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <Qthread>
#include <QMutexLocker>

client::client(const QStringList &args,QWidget *parent)//进行一些页面的布置，初始化分配空间
    : QMainWindow{parent}
    ,ui(new Ui::client)
{
    ui->setupUi(this);
    ip=args.at(1);
    port=args.at(2).toInt();
    color=args.at(3);
    /*
    ui->ip_edit->setText(ip);
    ui->port_edit->setText(QString::number(port));*/

    ui->send_button->setEnabled(false);
    ui->disconnect_button->setEnabled(false);
    ui->receive_edit->setReadOnly(true);
    ui->Confirm->setEnabled(false);
    ui->Winner_edit->setReadOnly(true);
    ui->goal_difference_edit->setReadOnly(true);
    ui->steps_edit->setReadOnly(true);

    socket = new NetworkSocket(new QTcpSocket(this), this); // 创建一个客户端对象，其父对象为当前窗口

    connect(socket->base(), &QTcpSocket::connected, this, &client::connected_successfully);  // connected 是客户端连接成功后发出的信号
    connect(ui->connect_button, &QPushButton::clicked, this, &client::connectToServer); // 连接服务器
    connect(ui->disconnect_button, &QPushButton::clicked, this, &client::disconnectFromServer); // 断开连接
    connect(ui->send_button, &QPushButton::clicked, this, &client::sendMessage); // 发送消息
    connect(socket, &NetworkSocket::receive, this, &client::receiveMessage);
    // receive 是客户端收到消息后发出的信号，receiveMessage 是处理这个信号的槽函数

    connect(ui->Confirm,&QPushButton::clicked, this, &client::sendReady);
    connect(socket,&NetworkSocket::receive,this,&client::receiveMove);
    connect(socket,&NetworkSocket::receive,this,&client::receiveReady);
    connect(socket,&NetworkSocket::receive,this,&client::receiveReject);
    connect(socket,&NetworkSocket::receive,this,&client::receiveEnd);
    connect(ui->lose,&QPushButton::clicked,this,&client::Giveup);
    connect(ui->useAI,&QPushButton::clicked,this,&client::useAI);
    connect(ui->StopAI,&QPushButton::clicked,this,&client::stopAI);
    for(int i=0;i<24;i++)
    {
        Chess[i]=new QLabel(this);
    }
    init();
    m_animation=new QPropertyAnimation(this);
    board_=std::const_pointer_cast<const SurakartaBoard>(game.board_);
    game_info_=std::const_pointer_cast<const SurakartaGameInfo>(game.game_info_);
    board_size_=game.board_size_;
    press.setX(-1);
    press.setY(-1);
    release.setX(-1);
    release.setY(-1);
    flag=0;
    t1=0;
    start=false;
    timer=new QTimer(this);
    timer->start(1000);
    MyAi=new AIthread;
    connect(MyAi,SIGNAL(sendReady()),this,SLOT(sendMove_Ai()));
    connect(m_animation,SIGNAL(finished()),this,SLOT(gameMove()));

    file.setFileName("./record/client/Team_15.txt");
    file.open(QIODevice::WriteOnly|QIODevice::Text);

    connectToServer();
    sendReady();


}

void client::connectToServer() {
    /*this->ip = ui->ip_edit->text();
    this->port = ui->port_edit->text().toInt();*/
    socket->hello(ip, port);                       // 连接服务器 ip:port
    this->socket->base()->waitForConnected(2000);  // 等待连接，2s 后超时
    // 你可以不阻塞（或者说连接在后台做），但这意味着你可以在连接成功前操作你的界面，比如发生消息，但此时还没建立连接，所以会出错
    // 建议懒人总是阻塞等待，写起来省事
}

void client::connected_successfully() {
    // 连接成功后，设置界面的状态
    ui->connect_button->setEnabled(false);
    ui->disconnect_button->setEnabled(true);
    ui->send_button->setEnabled(true);
    ui->port_edit->setReadOnly(true);
    ui->ip_edit->setText("Connected");
    ui->ip_edit->setReadOnly(true);
    ui->Confirm->setEnabled(true);
    // 这个程序中，连接成功后，发送一个消息给服务器，告诉服务器我已经准备好了,这不是网络中必须的操作，但是在游戏中，我们可能会规定这样的行为
}

void client::disconnectFromServer() {
    socket->send(NetworkData(OPCODE::LEAVE_OP, "", "", ""));
    // 这个程序中，客户端断开连接时要发送一个消息给服务器，这不是网络中必须的操作，这只是方便服务端知道客户端正常离开了（而不是异常断开）
    // 否则其实 tcp 连接中，服务端怎么检测客户端是否异常断开连接是一个有点烦人的问题，你可以搜索“qt tcp 怎么监测客户端有没有断开”
    socket->bye();
    // 在这个网络包里，断开只要调用 bye 就行了，但是在游戏的联机协议里，我们可能会规定其他行为，比如上面说的发送一个消息
    ui->connect_button->setEnabled(true);
    ui->disconnect_button->setEnabled(false);
    ui->send_button->setEnabled(false);
    ui->port_edit->setReadOnly(false);
    ui->ip_edit->setReadOnly(false);
    ui->ip_edit->setText(ip);
}


void client::sendMessage() {
    QString message = ui->send_edit->text();
    socket->send(NetworkData(OPCODE::CHAT_OP, "", message, "")); // 发送消息给服务端，是不是很简单
    ui->send_edit->clear();
}

void client::sendMove(NetworkData data){
    //实现输出记录
    QTextStream in(&file);
    in<<data.data1<<'-'<<data.data2<<' ';

    socket->send(data);
}

void client::sendReady()
{
    QString name="Team_15";

    QString room_id="1";
    Name=name;
    //发送颜色
    if(color=="BLACK")
    {
        PieceColor b=PieceColor::BLACK;
        socket->send(NetworkData(OPCODE::READY_OP,name,QString::fromStdString(std::to_string(static_cast<int>(b))),room_id));
    }
    else if(color=="WHITE")
    {
        PieceColor b=PieceColor::WHITE;
        socket->send(NetworkData(OPCODE::READY_OP,name,QString::fromStdString(std::to_string(static_cast<int>(b))),room_id));
    }
    ui->MyID_edit->clear();
    ui->MyColor_edit->clear();
    ui->MyRoom_edit->clear();
}

void client::receiveReady(NetworkData data)
{
    if(data.op==OPCODE::READY_OP)
    {
        game.StartGame();
        start=true;

        ui->ID_edit->setText(data.data1);
        ui->RoomID_edit->setText(data.data3);
        //为client设置颜色
        if(data.data2==QString::fromStdString(std::to_string(static_cast<int>(PieceColor::BLACK))))
        {
            c=PieceColor::BLACK;
            ui->Color_edit->setText("BLACK");
        }
        else
        {
            c=PieceColor::WHITE;
            ui->Color_edit->setText("WHITE");
        }
        //默认使用ai托管，方便测试
        useAI();
    }
}

void client::receiveReject(NetworkData data)
{
    //Ready信号不符合要求，退出页面
    if(data.op==OPCODE::REJECT_OP)
    {
        disconnectFromServer();
        this->close();
        Choose *ch=new Choose;
        ch->show();
    }
}

void client::receiveEnd(NetworkData data)
{
    if(data.op==OPCODE::END_OP)
    {
        //实现结尾的判断
        QTextStream in(&file);
        if(data.data2==QString::fromStdString(std::to_string(static_cast<int>(SurakartaEndReason::CHECKMATE))))in<<'C';
        if(data.data2==QString::fromStdString(std::to_string(static_cast<int>(SurakartaEndReason::ILLIGAL_MOVE))))in<<'I';
        if(data.data2==QString::fromStdString(std::to_string(static_cast<int>(SurakartaEndReason::RESIGN))))in<<'R';
        if(data.data2==QString::fromStdString(std::to_string(static_cast<int>(SurakartaEndReason::STALEMATE))))in<<'S';
        if(data.data2==QString::fromStdString(std::to_string(static_cast<int>(SurakartaEndReason::TIMEOUT))))in<<'T';
        in<<'#';
        start=false;
        ui->Winner_edit->setReadOnly(false);
        ui->goal_difference_edit->setReadOnly(false);
        ui->steps_edit->setReadOnly(false);
        //显示赢家信息等等
        if(data.data3==QString::fromStdString(std::to_string(static_cast<int>(PieceColor::BLACK))))
        {
            ui->Winner_edit->setText("BLACK");
        }
        else
        {
            ui->Winner_edit->setText("WHITE");
        }
        QString num=QString::number(game.game_info_->num_round_);
        ui->steps_edit->setText(num);
        ui->Winner_edit->setReadOnly(true);
        ui->goal_difference_edit->setReadOnly(true);
        ui->steps_edit->setReadOnly(true);
    }
}

void client::receiveMessage(NetworkData data) {
    if(data.op==OPCODE::CHAT_OP)
    {
        ui->receive_edit->setText(data.data2); // data 是收到的消息，我们显示出来
    }
}

void client::receiveMove(NetworkData data)
{
    if(data.op==OPCODE::MOVE_OP)
    {
        //实现输出记录
        QTextStream in(&file);
        in<<data.data1<<'-'<<data.data2<<' ';

        //实现对方棋子移动动画
        QPoint from=char2num(data.data1);
        QPoint to=char2num(data.data2);
        SurakartaMove move(from.x(),from.y(),to.x(),to.y(),game.game_info_->current_player_);
        SurakartaRuleManager *manager=new SurakartaRuleManager(game.board_,game.game_info_);
        if(manager->JudgeMove(move)==SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE)
        {
            for(int i=0;i<=23;i++)
            {
                if(from.x()==chess[i].x&&from.y()==chess[i].y&&chess[i].isDead)
                {
                    Move_non(i,from,to);
                    break;
                }
            }
        }
        else if(manager->JudgeMove(move)==SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE)
        {
            for(int i=0;i<=23;i++)
            {
                if(to.x()==chess[i].x&&to.y()==chess[i].y&&chess[i].isDead)
                {
                    Chess[i]->setVisible(false);
                    chess[i].isDead=0;
                    break;
                }
            }
            for(int i=0;i<=23;i++)
            {
                if(from.x()==chess[i].x&&from.y()==chess[i].y&&chess[i].isDead)
                {
                    //Move(i,from,to,manager->Route);
                    Move_non(i,from,to);
                    break;
                }
            }
        }
        else
        {
        }
        clientmove=move;
    }
}

void client::MyMove(NetworkData data)
{
    if(data.op==OPCODE::MOVE_OP)
    {
        //实现我方棋子移动动画
        QPoint from=char2num(data.data1);
        QPoint to=char2num(data.data2);
        SurakartaMove move(from.x(),from.y(),to.x(),to.y(),game.game_info_->current_player_);
        SurakartaRuleManager *manager=new SurakartaRuleManager(game.board_,game.game_info_);
        if(manager->JudgeMove(move)==SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE)
        {
            for(int i=0;i<=23;i++)
            {
                if(from.x()==chess[i].x&&from.y()==chess[i].y&&chess[i].isDead)
                {
                    Move_non(i,from,to);
                    break;
                }
            }
        }
        else if(manager->JudgeMove(move)==SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE)
        {
            for(int i=0;i<=23;i++)
            {
                if(to.x()==chess[i].x&&to.y()==chess[i].y&&chess[i].isDead)
                {
                    Chess[i]->setVisible(false);
                    chess[i].isDead=0;
                    break;
                }
            }
            for(int i=0;i<=23;i++)
            {
                if(from.x()==chess[i].x&&from.y()==chess[i].y&&chess[i].isDead)
                {
                    //Move(i,from,to,manager->Route);
                    Move_non(i,from,to);
                    break;
                }
            }
        }
        else
        {
        }
        clientmove=move;
    }
}

void client::paintEvent(QPaintEvent *)
{
    //画棋盘，以及标志出可以走的位置
    QPainter painter(this);
    int d=50;
    painter.setPen(QPen(Qt::darkBlue,3));
    for(int i=3;i<=8;i++)
    {
        painter.drawLine(QPoint(3*d,i*d),QPoint(8*d,i*d));
    }
    for(int j=3;j<=8;j++)
    {
        painter.drawLine(QPoint(j*d,3*d),QPoint(j*d,8*d));
    }
    for(int i=0;i<=1;i++)
    {
        QRect rectangle=QRect((2-i)*d,(2-i)*d,2*(i+1)*d,2*(i+1)*d);
        int startAngle=0;
        int spanAngle=270*16;
        painter.drawArc(rectangle,startAngle,spanAngle);
    }
    for(int i=0;i<=1;i++)
    {
        QRect rectangle=QRect((7-i)*d,(2-i)*d,2*(i+1)*d,2*(i+1)*d);
        int startAngle=-90*16;
        int spanAngle=270*16;
        painter.drawArc(rectangle,startAngle,spanAngle);
    }
    for(int i=0;i<=1;i++)
    {
        QRect rectangle=QRect((2-i)*d,(7-i)*d,2*(i+1)*d,2*(i+1)*d);
        int startAngle=90*16;
        int spanAngle=270*16;
        painter.drawArc(rectangle,startAngle,spanAngle);
    }
    for(int i=0;i<=1;i++)
    {
        QRect rectangle=QRect((7-i)*d,(7-i)*d,2*(i+1)*d,2*(i+1)*d);
        int startAngle=180*16;
        int spanAngle=270*16;
        painter.drawArc(rectangle,startAngle,spanAngle);
    }
    if(press.x()!=-1&&press.y()!=-1&&flag==1)
    {
        SurakartaRuleManager *manager=new SurakartaRuleManager(game.board_,game.game_info_);
        for(unsigned int i=0;i<board_size_;i++)
        {
            for(unsigned int j=0;j<board_size_;j++)
            {
                if((*board_)[press.x()][press.y()]->GetColor()==c)
                {
                    SurakartaMove move(press.x(),press.y(),i,j,c);
                    if(manager->JudgeMove(move)==SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE)
                    {
                        QColor color;
                        color.setRgb(0,255,0);
                        painter.setPen(QPen(color,5));
                        painter.drawLine(QPoint((3+i-0.5)*d,(3+j)*d),QPoint((3+i+0.5)*d,(3+j)*d));
                        painter.drawLine(QPoint((3+i)*d,(3+j-0.5)*d),QPoint((3+i)*d,(3+j+0.5)*d));
                    }
                    else if(manager->JudgeMove(move)==SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE)
                    {
                        QColor color;
                        color.setRgb(0,255,0);
                        painter.setPen(QPen(color,5));
                        painter.drawRect((3+i-0.5)*d,(3+j-0.5)*d,d,d);
                    }
                }
            }
        }
    }
    else if(release.x()!=-1&&release.y()!=-1&&flag==0)
    {
        if(t1==0)
        {
            QString from=num2char(press);
            QString to=num2char(release);
            MyMove(NetworkData(OPCODE::MOVE_OP,from,to,QString::fromStdString(std::to_string(static_cast<int>(c)))));
            sendMove(NetworkData(OPCODE::MOVE_OP,from,to,QString::fromStdString(std::to_string(static_cast<int>(c)))));
            t1=1;
        }
    }
}

void client::mouseReleaseEvent(QMouseEvent *ev)
{
    //返回鼠标点击的棋盘位置
    int d=50;
    if(start==true)
    {
        if(flag==0)
        {
            if(ev->button()==Qt::LeftButton)
            {
                int i=ev->x();
                int j=ev->y();
                for(int x=0;x<board_size_;x++)
                {
                    for(int y=0;y<board_size_;y++)
                    {
                        int dx=i-(3+x)*d;
                        int dy=j-(3+y)*d;
                        if(dx*dx+dy*dy<=(d/2)*(d/2))
                        {
                            press.setX(x);
                            press.setY(y);
                            flag=1;
                            update();
                        }
                    }
                }
            }
        }
        else if(flag==1)
        {
            if(ev->button()==Qt::LeftButton)
            {
                int i=ev->x();
                int j=ev->y();
                for(int x=0;x<board_size_;x++)
                {
                    for(int y=0;y<board_size_;y++)
                    {
                        int dx=i-(3+x)*d;
                        int dy=j-(3+y)*d;
                        if(dx*dx+dy*dy<=(d/2)*(d/2))
                        {
                            release.setX(x);
                            release.setY(y);
                            flag=0;
                            t1=0;
                            update();
                        }
                    }
                }
            }
        }
    }
}
//未实现的内容
void client::TimeButton()
{

}

void client::Clock1()
{

}

void client::Clock2()
{

}

void client::ResetNum()
{

}

void client::WinnerIs(PieceColor color)
{

}

void client::CleanWinner()
{

}

void client::Giveup()//认输按钮
{
    socket->send(NetworkData(OPCODE::RESIGN_OP,"","",""));
}

void client::Move_non(int i,QPoint from,QPoint to)
{
    int d=50;
    //动画的具体实现
    m_animation->setTargetObject(Chess[i]);
    m_animation->setEasingCurve(QEasingCurve::Linear);
    m_animation->setPropertyName("pos");    //指定动画属性名
    m_animation->setDuration(5);    //设置动画时间（单位：毫秒）
    m_animation->setStartValue(Chess[i]->pos());  //设置动画起始位置在label控件当前的pos
    m_animation->setEndValue(Chess[i]->pos() + QPoint((to.x()-from.x())*d, (to.y()-from.y())*d)); //设置动画结束位置
    m_animation->start();
    chess[i].x=to.x();
    chess[i].y=to.y();
}

void client::Move(int i,QPoint from,QPoint to,int array[4])
{
    int d=50;
    //旋吃动画，尚未完成
    m_animation->setTargetObject(Chess[i]);
    m_animation->setEasingCurve(QEasingCurve::Linear);
    m_animation->setPropertyName("pos");
    m_animation->setDuration(500);
    for(unsigned int i=0;i<4;i++)
    {
        if(array[i]==0)
        {

            break;
        }

    }
}

void client::init()
{
    int d=50;
    //初始化棋子的Label
    for(int i=0;i<=23;i++)
    {
        chess[i].initChess(i);
        Chess[i]->resize(d,d);
        Chess[i]->setPixmap(QPixmap(chess[i].chessPic));
        Chess[i]->setScaledContents(true);
        Chess[i]->setGeometry((3+chess[i].x-0.5)*d,(3+chess[i].y-0.5)*d,d,d);
        Chess[i]->show();
    }
}

QString client::num2char(QPoint a)
{
    //数字坐标转字符
    QString str;
    switch(a.x())
    {
    case 0:
        str+='A';
        break;
    case 1:
        str+='B';
        break;
    case 2:
        str+='C';
        break;
    case 3:
        str+='D';
        break;
    case 4:
        str+='E';
        break;
    case 5:
        str+='F';
        break;
    default:
        str+='G';
        break;
    }
    switch(a.y())
    {
    case 0:
        str+='1';
        break;
    case 1:
        str+='2';
        break;
    case 2:
        str+='3';
        break;
    case 3:
        str+='4';
        break;
    case 4:
        str+='5';
        break;
    case 5:
        str+='6';
        break;
    default:
        str+='7';
        break;
    }
    return str;
}
QPoint client::char2num(QString str)
{
    //字符转数字坐标
    QPoint p;
    if(str[0]=='A')
    {
        p.setX(0);
    }
    else if(str[0]=='B')
    {
        p.setX(1);
    }
    else if(str[0]=='C')
    {
        p.setX(2);
    }
    else if(str[0]=='D')
    {
        p.setX(3);
    }
    else if(str[0]=='E')
    {
        p.setX(4);
    }
    else if(str[0]=='F')
    {
        p.setX(5);
    }
    if(str[1]=='1')
    {
        p.setY(0);
    }
    else if(str[1]=='2')
    {
        p.setY(1);
    }
    else if(str[1]=='3')
    {
        p.setY(2);
    }
    else if(str[1]=='4')
    {
        p.setY(3);
    }
    else if(str[1]=='5')
    {
        p.setY(4);
    }
    else if(str[1]=='6')
    {
        p.setY(5);
    }
    return p;
}
client::~client() {
    delete ui;
    delete socket;
}

void client::useAI()
{
    //使用AI托管
    start=false;
    MyAi->m_isCanrun=true;
    MyAi->setGame(game);
    MyAi->My=c;
    MyAi->start();
}

void client::stopAI()
{
    //停止AI托管
    MyAi->stopImmediately();
    start=true;
}

void client::sendMove_Ai()
{
    //实现AI发送移动信息给sever
    QMutexLocker locker(&MyAi->m_lock);
    QPoint from(MyAi->m_move.from.x,MyAi->m_move.from.y);
    QPoint to(MyAi->m_move.to.x,MyAi->m_move.to.y);
    QString From=num2char(from);
    QString To=num2char(to);
    MyMove(NetworkData(OPCODE::MOVE_OP,From,To,QString::fromStdString(std::to_string(static_cast<int>(c)))));
    sendMove(NetworkData(OPCODE::MOVE_OP,From,To,QString::fromStdString(std::to_string(static_cast<int>(c)))));


}

void client::gameMove()
{
    //游戏信息的更新，在接收到动画完成的信号后才会进行
    QMutexLocker locker(&MyAi->m_lock);
    game.Move(clientmove);
    if(MyAi->m_isCanrun==true)
    {
        /*SurakartaIllegalMoveReason move_reason = MyAi->game.rule_manager_->JudgeMove(move);
            auto [end_reason, winner] = MyAi->game.rule_manager_->JudgeEnd(move_reason);

            MyAi->game.UpdateGameInfo(move_reason, end_reason, winner);*/
        MyAi->t=0;
    }
    locker.unlock();
    update();
}
