#ifndef PVPGAME_H
#define PVPGAME_H

#include <QWidget>
#include <QPainter>
#include <choose.h>
#include <surakarta_piece.h>
#include <surakarta_game.h>
#include <chess.h>
#include <QMouseEvent>
#include <QLCDNumber>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QTimer>
#include <QPropertyAnimation>
#define BOARD_SIZE 6
namespace Ui {
class PVPgame;
}

class PVPgame : public QWidget
{
    Q_OBJECT

public:
    explicit PVPgame(QWidget *parent = nullptr);
    ~PVPgame();
    void paintEvent(QPaintEvent *);
    SurakartaGame game;//新游戏
    std::shared_ptr<const SurakartaBoard> board_;
    std::shared_ptr<const SurakartaGameInfo> game_info_;
    unsigned int board_size_;
    QPoint press;//初始位置
    QPoint release;//目标位置
    int flag,flag2;//用来切换处理初始位置和目标位置
    QLCDNumber *lcd_B;//黑色倒计时显示器
    QLCDNumber *lcd_W;//白色倒计时显示器
    QLCDNumber *lcd_num;//步数显示器
    QTimer *timer;//为调用表的时间间隔计时
    QLabel *label;
    QLabel *winner_B;//显示黑色赢
    QLabel *winner_W;//显示白色赢
    QPushButton *button;
    QPushButton lose_W;//认输
    QPushButton lose_B;//认输
    QTextEdit *text;//在这里输入更改的时间限制
    int minute,m_;//分
    int second,s_;//秒
    Chess chess[24];//棋子
    QLabel *Chess[24];//棋子显示
    void init();//初始化棋子
    QPropertyAnimation *m_animation;//动画
private:
    Ui::PVPgame *ui;
private slots:
    void BackButton();//返回choose界面
    void NewgameButton();//新游戏
    void mouseReleaseEvent(QMouseEvent *ev);//返回鼠标点击位置
    void TimeButton();//确认输入的更改的时间限制
    void Clock1();//黑色计时器
    void Clock2();//白色计时器
    void ResetNum();//重置步数
    void WinnerIs(PieceColor color);//显示谁赢了
    void CleanWinner();
    void LoseBlack();//认输
    void LoseWhite();//认输
    void Move_non(int i);//不吃子动画
    void Move(int i);//吃子动画
};



#endif // PVPGAME_H
