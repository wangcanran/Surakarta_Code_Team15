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
    bool m_isCanrun;
    void stopImmediately();
    QMutex m_lock;
    SurakartaMove m_move;
    PieceColor My;
    int t;
signals:
    void sendReady();
protected:
    void run();
};

#endif // AITHREAD_H
