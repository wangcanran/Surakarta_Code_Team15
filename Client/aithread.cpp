#include "aithread.h"
#include "surakarta_game.h"
#include "surakarta_board.h"
#include "surakarta_common.h"
#include "surakarta_rule_manager.h"
#include "surakarta_agent_mine.h"
#include <QMutexLocker>
#include <QThread>

AIthread::AIthread(QObject *parent):QThread(parent)
{
    QMutexLocker locker(&m_lock);
    t=0;
    m_isCanrun=false;
    game.StartGame();
}

void AIthread::run()
{
    std::shared_ptr<SurakartaRuleManager> rule_manager = std::make_shared<SurakartaRuleManager>(game.GetBoard(), game.GetGameInfo());
    std::shared_ptr<SurakartaAgentMine> agent = std::make_shared<SurakartaAgentMine>(game.GetBoard(), game.GetGameInfo(), rule_manager);
    game.SetRuleManager(rule_manager);
    //只要游戏没结束就一直进行
    while (!game.IsEnd()) {
        QMutexLocker locker(&m_lock);
        if(m_isCanrun==false)
        {
            locker.unlock();
            break;
        }
        //只有自己的回合才调用AI
        if(game.game_info_->current_player_==My&&t==0)
        {
            auto move = agent->CalculateMove();
            m_move=move;
            locker.unlock();
            //QThread::sleep(1);
            emit sendReady();
            t=1;
            QThread::sleep(1);
        }
    }
}

void AIthread::stopImmediately()//利用改变参数的方法来停止线程
{
    QMutexLocker locker(&m_lock);
    m_isCanrun=false;
}

void AIthread::setGame(SurakartaGame Game)//传入初始游戏信息
{
    QMutexLocker locker(&m_lock);
    game=Game;
}
