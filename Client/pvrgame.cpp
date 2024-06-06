#include "pvrgame.h"
#include "ui_pvrgame.h"

PVRgame::PVRgame(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PVRgame)
{
    ui->setupUi(this);
}

PVRgame::~PVRgame()
{
    delete ui;
}
