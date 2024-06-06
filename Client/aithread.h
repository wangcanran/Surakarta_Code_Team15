#ifndef AITHREAD_H
#define AITHREAD_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include "surakarta_game.h"
#include "surakarta_board.h"
#include "surakarta_common.h"
#include "surakarta_rule_manager.h"
#include "surakarta_piece.h"

class AIthread:public QThread
{
    Q_OBJECT
public:
    explicit AIthread(QObject *parent = nullptr);
    SurakartaGame game;
    void setGame(SurakartaGame Game);
    bool m_isCanrun;//控制线程运行的条件变量
    void stopImmediately();//停止线程
    QMutex m_lock;//互斥锁，保证信息安全
    SurakartaMove m_move;//move信息
    PieceColor My;//我的颜色
    int t;//控制只发送一次move的条件变量
signals:
    void sendReady();
protected:
    void run();
};

#endif // AITHREAD_H
