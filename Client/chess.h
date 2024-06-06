#ifndef CHESS_H
#define CHESS_H

#include <QWidget>
#include <QFile>
#include <QTextStream>

namespace Ui {
class Chess;
}

class Chess : public QWidget
{
    Q_OBJECT

public:
    explicit Chess(QWidget *parent = nullptr);
    ~Chess();

    enum TYPE{BLACK,WHITE};

private:
    Ui::Chess *ui;
signals:
public:
    void initChess(const int id);//棋子初始化
public:
    int x;//坐标
    int y;
    int chessType;//棋子颜色
    int chessID;//棋子编号
    bool isDead;//棋子是否被吃
    QString chessPic;//棋子图片
};

#endif // CHESS_H
