#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include "networkdata.h"
#include "networksocket.h"
#include "networkserver.h"
#include "surakarta_piece.h"
#include "surakarta_game.h"
#include "aithread.h"
#include <chess.h>
#include <networkdata.h>
#include <QMouseEvent>
#include <QLCDNumber>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QPropertyAnimation>
#include <QTextStream>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui {
class client;
}
QT_END_NAMESPACE

class client : public QMainWindow
{
    Q_OBJECT
public:
    explicit client(const QStringList &args,QWidget *parent = nullptr);
    ~client();
    PieceColor c;
    void sendMove(NetworkData);
    SurakartaGame game;
    QPoint char2num(QString);
    void paintEvent(QPaintEvent *);
    std::shared_ptr<const SurakartaBoard> board_;
    std::shared_ptr<const SurakartaGameInfo> game_info_;
    unsigned int board_size_;
    QPoint press;
    QPoint release;
    int flag,flag2,t1,t2;
    QLCDNumber *lcd_B;
    QLCDNumber *lcd_W;
    QLCDNumber *lcd_num;
    QTimer *timer;
    QLabel *label;
    QLabel *winner_B;
    QLabel *winner_W;
    QPushButton *button;
    QTextEdit *text;
    int minute,m_;
    int second,s_;
    Chess chess[24];
    QLabel *Chess[24];
    void init();
    QPropertyAnimation *m_animation;
    QString num2char(QPoint);
    bool start;
    QString Name;
    AIthread *MyAi;
    SurakartaMove clientmove;
    QFile file;
    QString color;

private:
    int port ;
    QString ip ;
    Ui::client *ui;
    NetworkSocket *socket;

private slots:
    void connected_successfully();//连接成功
    void connectToServer();//与server连接
    void disconnectFromServer();//断开连接
    void sendMessage();//发消息
    void receiveMessage(NetworkData);//接收消息
    void sendReady();//发送Ready_OP
    void receiveReady(NetworkData);//接收Ready_OP并开始游戏
    void receiveMove(NetworkData);//接收MOVE_OP并启动画
    void MyMove(NetworkData);//启动自己的行棋动画
    void receiveReject(NetworkData);//拒绝一起游戏
    void receiveEnd(NetworkData);//接收END_OP

    void mouseReleaseEvent(QMouseEvent *ev);
    void TimeButton();
    void Clock1();
    void Clock2();
    void ResetNum();
    void WinnerIs(PieceColor color);
    void CleanWinner();
    void Giveup();
    void Move_non(int i,QPoint from,QPoint to);//不吃子动画实现
    void Move(int i,QPoint from,QPoint to,int array[4]);//吃子动画实现
    void useAI();//AI托管
    void stopAI();//结束AI托管
    void sendMove_Ai();//发送MOVE的信息
    void gameMove();//更新棋盘
};

#endif // CLIENT_H
