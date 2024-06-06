#include "pvpgame.h"
#include "ui_pvpgame.h"
#include "choose.h"
#include "ui_choose.h"
#include <QPainter>
#include <QRectF>
#include <QRect>
#include <QPushButton>
#include <QWidget>
#include "surakarta_piece.h"
#include "surakarta_game.h"
#include "surakarta_board.h"
#include "surakarta_common.h"
#include "chess.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <QPixmap>
#include <QPainterPath>
#include <QString>
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
PVPgame::PVPgame(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PVPgame)
{
    setWindowFlags(Qt::Window);
    ui->setupUi(this);
    this->resize(1000,600);
    QPushButton *back=new QPushButton("Return",this);
    back->setGeometry(0,0,50,50);
    QPushButton *newgame=new QPushButton("New Game",this);
    newgame->setGeometry(900,0,100,50);
    QPushButton *lose_B=new QPushButton("Concede",this);
    QPushButton *lose_W=new QPushButton("Concede",this);
    lose_B->setGeometry(700,20,80,50);
    lose_W->setGeometry(700,470,80,50);
    connect(back,SIGNAL(clicked()),this,SLOT(BackButton()));
    connect(newgame,SIGNAL(clicked()),this,SLOT(NewgameButton()));
    connect(lose_B,SIGNAL(clicked()),this,SLOT(LoseBlack()));
    connect(lose_W,SIGNAL(clicked()),this,SLOT(LoseWhite()));
    winner_B=new QLabel(this);
    winner_W=new QLabel(this);
    label=new QLabel("The maximum duration(s):",this);
    QFont font("Arial",12,QFont::Bold);
    label->setFont(font);
    text=new QTextEdit(this);
    button=new QPushButton("Enter",this);
    label->setGeometry(800,300,200,100);
    text->setGeometry(800,400,200,100);
    button->setGeometry(800,500,200,50);
    connect(button,SIGNAL(clicked()),this,SLOT(TimeButton()));
    timer=new QTimer(this);
    timer->start(1000);
    minute=10;
    second=0;
    m_=minute;
    s_=second;
    lcd_B=new QLCDNumber(this);
    lcd_W=new QLCDNumber(this);
    lcd_num=new QLCDNumber(this);
    lcd_B->setDigitCount(5);
    lcd_B->setMode(QLCDNumber::Dec);
    lcd_B->setSegmentStyle(QLCDNumber::Flat);
    lcd_B->setGeometry(550,20,150,50);
    lcd_W->setDigitCount(5);
    lcd_W->setMode(QLCDNumber::Dec);
    lcd_W->setSegmentStyle(QLCDNumber::Flat);
    lcd_W->setGeometry(550,470,150,50);
    lcd_num->setDigitCount(5);
    lcd_num->setMode(QLCDNumber::Dec);
    lcd_num->setSegmentStyle(QLCDNumber::Flat);
    lcd_num->setGeometry(550,250,150,50);
    winner_B->setGeometry(550,140,150,50);
    winner_W->setGeometry(550,360,150,50);
    QFont font1("Arial",20,QFont::Bold);
    winner_B->setFont(font1);
    winner_W->setFont(font1);
    QString num=QString::number(game.game_info_->num_round_);
    QString s_minute=QString::number(minute);
    QString s_second=QString::number(second);
    if(s_minute.length()==1)
    {
        s_minute="0"+s_minute;
    }
    if(s_second.length()==1)
    {
        s_second="0"+s_second;
    }
    QString disp=s_minute+':'+s_second;
    lcd_W->display(disp);
    lcd_B->display(disp);
    lcd_num->display(num+"r");
    for(int i=0;i<=23;i++)
    {
        Chess[i]=new QLabel(this);
    }
    init();
    m_animation=new QPropertyAnimation(this);
    game.StartGame();
    board_=std::const_pointer_cast<const SurakartaBoard>(game.board_);
    game_info_=std::const_pointer_cast<const SurakartaGameInfo>(game.game_info_);
    board_size_=game.board_size_;
    press.setX(-1);
    press.setY(-1);
    release.setX(-1);
    release.setY(-1);
    flag=0;
    flag2=0;
}

PVPgame::~PVPgame()
{
    delete ui;
}

void PVPgame::paintEvent(QPaintEvent*)
{
    //初始化绘制棋盘
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
    //可以走的位置的标注
    if(press.x()!=-1&&press.y()!=-1&&flag==1)
    {
        SurakartaRuleManager *manager=new SurakartaRuleManager(game.board_,game.game_info_);
        for(unsigned int i=0;i<board_size_;i++)
        {
            for(unsigned int j=0;j<board_size_;j++)
            {
                if((*board_)[press.x()][press.y()]->GetColor()==PieceColor::BLACK)
                {
                SurakartaMove move(press.x(),press.y(),i,j,PieceColor::BLACK);
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
                else if((*board_)[press.x()][press.y()]->GetColor()==PieceColor::WHITE)
                {
                    SurakartaMove move(press.x(),press.y(),i,j,PieceColor::WHITE);
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
    //棋子移动的动画和棋盘信息更新
    else if(release.x()!=-1&&release.y()!=-1&&flag==0)
    {
        if((*board_)[press.x()][press.y()]->GetColor()==PieceColor::BLACK)
        {
            SurakartaMove move(press.x(),press.y(),release.x(),release.y(),PieceColor::BLACK);
            SurakartaRuleManager *manager=new SurakartaRuleManager(game.board_,game.game_info_);
            if(manager->JudgeMove(move)==SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE||manager->JudgeMove(move)==SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE)
            {
                s_=second;
                m_=minute;
                connect(timer,SIGNAL(timeout()),this,SLOT(Clock2()));
                disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock1()));
                ResetNum();
            }
            if(manager->JudgeMove(move)==SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE)
            {
                for(int i=0;i<=11;i++)
                {
                    if(press.x()==chess[i].x&&press.y()==chess[i].y&&chess[i].isDead)
                    {
                        Move_non(i);
                        break;
                    }
                }
            }
            else if(manager->JudgeMove(move)==SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE)
            {
                for(int i=0;i<=11;i++)
                {
                    if(press.x()==chess[i].x&&press.y()==chess[i].y&&chess[i].isDead)
                    {
                        Move_non(i);
                        break;
                    }
                }
                for(int i=12;i<=23;i++)
                {
                    if(release.x()==chess[i].x&&release.y()==chess[i].y&&chess[i].isDead)
                    {
                        Chess[i]->setVisible(false);
                        chess[i].isDead=0;
                        break;
                    }
                }
            }
            game.Move(move);
            if(game.IsEnd())
            {
                disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock1()));
                disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock2()));
                if(game.game_info_->winner_==PieceColor::BLACK)
                {
                    WinnerIs(PieceColor::BLACK);
                }
                else
                {
                    WinnerIs(PieceColor::WHITE);
                }
            }
            update();
        }
        else if((*board_)[press.x()][press.y()]->GetColor()==PieceColor::WHITE)
        {
            SurakartaMove move(press.x(),press.y(),release.x(),release.y(),PieceColor::WHITE);
            SurakartaRuleManager *manager=new SurakartaRuleManager(game.board_,game.game_info_);
            if(manager->JudgeMove(move)==SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE||manager->JudgeMove(move)==SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE)
            {
                s_=second;
                m_=minute;
                connect(timer,SIGNAL(timeout()),this,SLOT(Clock1()));
                disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock2()));
                ResetNum();
            }
            if(manager->JudgeMove(move)==SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE)
            {
                for(int i=12;i<=23;i++)
                {
                    if(press.x()==chess[i].x&&press.y()==chess[i].y)
                    {
                        Move_non(i);
                        break;
                    }
                }
            }
            else if(manager->JudgeMove(move)==SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE)
            {
                for(int i=12;i<=23;i++)
                {
                    if(press.x()==chess[i].x&&press.y()==chess[i].y&&chess[i].isDead)
                    {
                        Move_non(i);
                        break;
                    }
                }
                for(int i=0;i<=11;i++)
                {
                    if(release.x()==chess[i].x&&release.y()==chess[i].y&&chess[i].isDead)
                    {
                        Chess[i]->setVisible(false);
                        chess[i].isDead=0;
                        break;
                    }
                }
            }
            game.Move(move);
            if(game.IsEnd())
            {
                disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock1()));
                disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock2()));
                if(game.game_info_->winner_==PieceColor::BLACK)
                {
                    WinnerIs(PieceColor::BLACK);
                }
                else
                {
                    WinnerIs(PieceColor::WHITE);
                }
            }
            update();
        }
    }
}

void PVPgame::BackButton()
{
    //返回choose界面
    this->close();
    Choose *ch=new Choose;
    ch->show();
}
void PVPgame::TimeButton()
{
    //时间初始化，计时器的显示
    QString strtime=text->toPlainText();
    int time=strtime.toInt();
    minute=time/60;
    second=time%60;
    s_=second;
    m_=minute;
    text->clear();
    QString s_minute=QString::number(minute);
    QString s_second=QString::number(second);
    if(s_minute.length()==1)
    {
        s_minute="0"+s_minute;
    }
    if(s_second.length()==1)
    {
        s_second="0"+s_second;
    }
    QString disp=s_minute+':'+s_second;
    lcd_W->display(disp);
    lcd_B->display(disp);
}

void PVPgame::NewgameButton()
{
    //实现再来一局
    game.StartGame();
    flag=0;
    press.setX(-1);
    press.setY(-1);
    release.setX(-1);
    release.setY(-1);
    board_=std::const_pointer_cast<const SurakartaBoard>(game.board_);
    game_info_=std::const_pointer_cast<const SurakartaGameInfo>(game.game_info_);
    s_=second;
    m_=minute;
    QString s_minute=QString::number(minute);
    QString s_second=QString::number(second);
    if(s_minute.length()==1)
    {
        s_minute="0"+s_minute;
    }
    if(s_second.length()==1)
    {
        s_second="0"+s_second;
    }
    QString disp=s_minute+':'+s_second;
    lcd_W->display(disp);
    lcd_B->display(disp);
    disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock1()));
    disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock2()));
    game.game_info_->Reset();
    CleanWinner();
    ResetNum();
    init();
    update();
}


void PVPgame::mouseReleaseEvent(QMouseEvent *ev)
{
    //实现传输鼠标点击信息
    int d=50;
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
                    update();
                }
                }
            }
        }
    }
}

void PVPgame::Clock1()
{
    //计时器
    if((m_!=0)|(s_!=0)){
        if(s_==0){
            s_=59;
            m_--;}
        else
            s_--;
    }
    QString s_minute=QString::number(m_);
    QString s_second=QString::number(s_);
    if(s_minute.length()==1)
    {
        s_minute="0"+s_minute;
    }
    if(s_second.length()==1)
    {
        s_second="0"+s_second;
    }
    QString disp=s_minute+':'+s_second;
    lcd_B->display(disp);
    if(m_==0&&s_==0)
    {
        game.game_info_->winner_=PieceColor::WHITE;
        WinnerIs(PieceColor::WHITE);
        disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock1()));
        disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock2()));
    }
}

void PVPgame::Clock2()
{
    //计时器
    if((minute!=0)|(second!=0)){
        if(s_==0){
            s_=59;
            m_--;}
        else
            s_--;
    }
    QString s_minute=QString::number(m_);
    QString s_second=QString::number(s_);
    if(s_minute.length()==1)
    {
        s_minute="0"+s_minute;
    }
    if(s_second.length()==1)
    {
        s_second="0"+s_second;
    }
    QString disp=s_minute+':'+s_second;
    lcd_W->display(disp);
    if(m_==0&&s_==0)
    {
        game.game_info_->winner_=PieceColor::BLACK;
        WinnerIs(PieceColor::BLACK);
        disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock1()));
        disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock2()));
    }
}

void PVPgame::ResetNum()
{
    //轮数重置
    QString num=QString::number(game.game_info_->num_round_);
    lcd_num->display(num+"r");
}

void PVPgame::WinnerIs(PieceColor color)
{
    //显示赢家
    if(color==PieceColor::BLACK)
    {
        QPalette pe;
        pe.setColor(QPalette::WindowText,Qt::red);
        winner_B->setPalette(pe);
        winner_B->setText("WIN!");
    }
    else
    {
        QPalette pe;
        pe.setColor(QPalette::WindowText,Qt::red);
        winner_W->setPalette(pe);
        winner_W->setText("WIN!");
    }
}

void PVPgame::CleanWinner()
{
    //清空赢家
    winner_B->setText("");
    winner_W->setText("");
}

void PVPgame::LoseBlack()
{
    //认输
    game.game_info_->winner_=PieceColor::WHITE;
    WinnerIs(PieceColor::WHITE);
    disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock1()));
    disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock2()));
}

void PVPgame::LoseWhite()
{
    //认输
    game.game_info_->winner_=PieceColor::BLACK;
    WinnerIs(PieceColor::BLACK);
    disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock1()));
    disconnect(timer,SIGNAL(timeout()),this,SLOT(Clock2()));
}

void PVPgame::init()
{
    int d=50;
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

void PVPgame::Move_non(int i)
{
    //不吃子动画的具体实现
    int d=50;
    m_animation->setTargetObject(Chess[i]);
    m_animation->setEasingCurve(QEasingCurve::Linear);
    m_animation->setPropertyName("pos");    //指定动画属性名
    m_animation->setDuration(500);    //设置动画时间（单位：毫秒）
    m_animation->setStartValue(Chess[i]->pos());  //设置动画起始位置在label控件当前的pos
    m_animation->setEndValue(Chess[i]->pos() + QPoint((release.x()-press.x())*d, (release.y()-press.y())*d)); //设置动画结束位置
    m_animation->start();
    chess[i].x=release.x();
    chess[i].y=release.y();
}

void PVPgame::Move(int i)
{
    int d=50;
}
