#include "choose.h"
#include "ui_choose.h"
#include "pvpgame.h"
#include "ui_pvpgame.h"
#include "pvrgame.h"
#include "ui_pvrgame.h"
#include "client.h"
#include "ui_client.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
Choose::Choose(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Choose)
{
    ui->setupUi(this);
    QVBoxLayout* lay=new QVBoxLayout(this);
    lay->addWidget(label=new QLabel("SURAKARTA GAME"));
    QFont font("Arial",18,QFont::Bold);
    label->setFont(font);
    label->setAlignment((Qt::AlignCenter));
    lay->addWidget(_button[0]=new QPushButton("PVP"));
    lay->addWidget(_button[1]=new QPushButton("PVR"));
    lay->addWidget(_button[2]=new QPushButton("Quit"));
    lay->addWidget(_button[3]=new QPushButton("Netgame"));
    connect(_button[0],SIGNAL(clicked()),this,SLOT(pushButton1_clicked()));
    connect(_button[1],SIGNAL(clicked()),this,SLOT(pushButton2_clicked()));
    connect(_button[2],SIGNAL(clicked()),this,SLOT(pushButton3_clicked()));
    connect(_button[3],SIGNAL(clicked()),this,SLOT(pushButton4_clicked()));
}

Choose::~Choose()
{
    delete ui;
}
void Choose::pushButton1_clicked()
{
    PVPgame *GAME=new PVPgame;
    GAME->show();
    this->hide();
}
void Choose::pushButton2_clicked()
{
    PVRgame *GAME=new PVRgame;
    GAME->show();
    this->hide();
}
void Choose::pushButton3_clicked()
{
    this->close();
}

void Choose::pushButton4_clicked()
{
    client *Client=new client;
    Client->show();
    this->close();
}
