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
    void connected_successfully();
    void connectToServer();
    void disconnectFromServer();
    void sendMessage();
    void receiveMessage(NetworkData);
    void sendReady();
    void receiveReady(NetworkData);
    void receiveMove(NetworkData);
    void MyMove(NetworkData);
    void receiveReject(NetworkData);
    void receiveEnd(NetworkData);

    void mouseReleaseEvent(QMouseEvent *ev);
    void TimeButton();
    void Clock1();
    void Clock2();
    void ResetNum();
    void WinnerIs(PieceColor color);
    void CleanWinner();
    void Giveup();
    void Move_non(int i,QPoint from,QPoint to);
    void Move(int i,QPoint from,QPoint to,int array[4]);
    void useAI();
    void stopAI();
    void sendMove_Ai();
    void gameMove();
};

#endif // CLIENT_H
