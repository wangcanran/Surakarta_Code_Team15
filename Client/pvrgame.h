#ifndef PVRGAME_H
#define PVRGAME_H

#include <QWidget>

namespace Ui {
class PVRgame;
}

class PVRgame : public QWidget
{
    Q_OBJECT

public:
    explicit PVRgame(QWidget *parent = nullptr);
    ~PVRgame();

private:
    Ui::PVRgame *ui;
};

#endif // PVRGAME_H
