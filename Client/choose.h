#ifndef CHOOSE_H
#define CHOOSE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <pvpgame.h>
#include <pvrgame.h>
namespace Ui {
class Choose;
}

class Choose : public QWidget
{
    Q_OBJECT

public:
    explicit Choose(QWidget *parent = nullptr);
    ~Choose();

private:
    Ui::Choose *ui;
    QLabel *label;
    QPushButton *_button[4];
private slots:
    void pushButton1_clicked();
    void pushButton2_clicked();
    void pushButton3_clicked();
    void pushButton4_clicked();
};

#endif // CHOOSE_H
