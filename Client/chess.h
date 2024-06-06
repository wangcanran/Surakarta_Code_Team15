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
    void initChess(const int id);
public:
    int x;
    int y;
    int chessType;
    int chessID;
    bool isDead;
    QString chessPic;
};

#endif // CHESS_H
