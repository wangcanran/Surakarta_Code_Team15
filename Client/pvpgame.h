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
    SurakartaGame game;
    std::shared_ptr<const SurakartaBoard> board_;
    std::shared_ptr<const SurakartaGameInfo> game_info_;
    unsigned int board_size_;
    QPoint press;
    QPoint release;
    int flag,flag2;
    QLCDNumber *lcd_B;
    QLCDNumber *lcd_W;
    QLCDNumber *lcd_num;
    QTimer *timer;
    QLabel *label;
    QLabel *winner_B;
    QLabel *winner_W;
    QPushButton *button;
    QPushButton lose_W;
    QPushButton lose_B;
    QTextEdit *text;
    int minute,m_;
    int second,s_;
    Chess chess[24];
    QLabel *Chess[24];
    void init();
    QPropertyAnimation *m_animation;
private:
    Ui::PVPgame *ui;
private slots:
    void BackButton();
    void NewgameButton();
    void mouseReleaseEvent(QMouseEvent *ev);
    void TimeButton();
    void Clock1();
    void Clock2();
    void ResetNum();
    void WinnerIs(PieceColor color);
    void CleanWinner();
    void LoseBlack();
    void LoseWhite();
    void Move_non(int i);
    void Move(int i);
};



#endif // PVPGAME_H
