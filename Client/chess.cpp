#include "chess.h"
#include "ui_chess.h"

Chess::Chess(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Chess)
{
    ui->setupUi(this);
}

Chess::~Chess()
{
    delete ui;
}


void Chess::initChess(const int id)//初始化棋子
{
    if(id>=24||id<0)
    {
        return;
    }
    chessID=id;
    isDead=1;
    if(id<12)
    {
        chessType=BLACK;
        chessPic=":/new/prefix1/black.png";
    }
    if(id>=12)
    {
        chessType=WHITE;
        chessPic=":/new/prefix1/white.png";
    }
    switch (id) {
    case 0:y=0,x=0;break;
    case 1:y=0,x=1;break;
    case 2:y=0,x=2;break;
    case 3:y=0,x=3;break;
    case 4:y=0,x=4;break;
    case 5:y=0,x=5;break;
    case 6:y=1,x=0;break;
    case 7:y=1,x=1;break;
    case 8:y=1,x=2;break;
    case 9:y=1,x=3;break;
    case 10:y=1,x=4;break;
    case 11:y=1,x=5;break;

    case 12:y=4,x=0;break;
    case 13:y=4,x=1;break;
    case 14:y=4,x=2;break;
    case 15:y=4,x=3;break;
    case 16:y=4,x=4;break;
    case 17:y=4,x=5;break;
    case 18:y=5,x=0;break;
    case 19:y=5,x=1;break;
    case 20:y=5,x=2;break;
    case 21:y=5,x=3;break;
    case 22:y=5,x=4;break;
    case 23:y=5,x=5;break;
    }
}
