#include "surakarta_agent_mine.h"
#include "surakarta_agent_random.h"
#include <cmath>
#include <cassert>
#include <memory>


constexpr int MAX_ITER = 100;

constexpr double SCALAR = 1.0;
constexpr double WIN_SCORE = 100.0;
constexpr double LOSE_SCORE = -100.0;


SurakartaMove SurakartaAgentMine::CalculateMove() {
    // TODO: Implement your own ai here.
    auto simple_board = std::make_unique<const SurakartaSimpleBoard>(SurakartaSimpleBoard(*board_));
    SurakartaPlayer player = game_info_->current_player_;

    MNode* root = new MNode(std::move(simple_board), player);

    // MCTS
    //这是主函数，用于计算并返回AI的下一步移动。它创建了一个树节点的根节点，然后通过MCTS算法进行多次迭代来选择最佳移动。
    for (int i = 0; i < MAX_ITER; i++) {
        MNode* selected_node = root->Select();
        if (selected_node != nullptr) {
            MNode* new_node = selected_node->Expand();
            SurakartaPlayer winner = new_node->Simulate();
            selected_node->BackPropagate(winner);
        }
    }
    MNode* best_child = root->MaxUCBChild(0);
    SurakartaMove move = best_child->parent_move_;

    delete root;

    return move;
}

/////////////////////////// MCTS ///////////////////////////
//构造函数，初始化MNode对象，包括棋盘状态和当前玩家。
MNode::MNode(std::unique_ptr<const SurakartaSimpleBoard> simple_board, const SurakartaPlayer player) :
    simple_board_(std::move(simple_board)),
    player_(player),
    legal_moves_(simple_board_->GetAllLegalMoves(player_)) {}


MNode* MNode::Select() {//选择函数，用于在MCTS树中选择一个节点进行扩展。它通过UCB（Upper Confidence Bound）策略来选择节点。
    MNode* current = this;
    while (true) {
        if (current->simple_board_->IsEnd()) {
            return nullptr;
        }

        if (current->IsFullyExpanded()) {
            current = current->MaxUCBChild(SCALAR);
        }
        else {
            return current;
        }
    }
}


MNode* MNode::Expand() {//扩展函数，用于在MCTS树中添加一个新的子节点。它会选择一个合法的移动，并创建一个新的棋盘状态。
    assert(!IsFullyExpanded());

    SurakartaMove move = legal_moves_[children_.size()];
    auto simple_board_copy = std::make_unique<SurakartaSimpleBoard>(*simple_board_);
    simple_board_copy->UnsafeMove(move);
    SurakartaPlayer next_player = ReverseColor(player_);

    MNode* child = new MNode(std::move(simple_board_copy), next_player);
    child->parent_ = this;
    child->parent_move_ = move;
    children_.push_back(child);

    return child;
}


SurakartaPlayer MNode::Simulate() {//利用随机AI演算最后的胜者
    SurakartaPlayer winner = simple_board_->RunRandomGame(player_);
    return winner;
}


void MNode::BackPropagate(SurakartaPlayer winner) {//反向传播函数，将模拟的结果从叶节点向上传递到根节点，更新每个节点的访问次数和奖励分数。
    MNode* current = this;
    while (current != nullptr) {
        current->visits_++;
        if (current->parent_move_.player == winner) {
            current->reward_ += WIN_SCORE;
        }
        else {
            current->reward_ += LOSE_SCORE;
        }
        current = current->parent_;
    }
}


bool MNode::IsFullyExpanded() {//判断节点是否已经完全扩展，即所有合法的移动都已经生成了子节点。
    return children_.size() == legal_moves_.size();
}


double MNode::UCB(double C) {//计算节点的UCB值，用于选择节点。
    assert(parent_ != nullptr);
    assert(visits_ > 0);
    return 1.0 * reward_ / visits_ + C * sqrt(2.0 * log(1.0 * parent_->visits_ / visits_));
}


MNode* MNode::MaxUCBChild(double C) {//找到具有最大UCB值的子节点。
    double max_ucb = -INFINITY;
    MNode* max_ucb_child = nullptr;
    for (auto child : children_) {
        if (child->visits_ == 0) {
            return child;
        }
        double ucb = child->UCB(C);
        if (ucb > max_ucb) {
            max_ucb = ucb;
            max_ucb_child = child;
        }
    }
    return max_ucb_child;
}


/////////////////////////// SimpleBoard ///////////////////////////

SurakartaSimpleBoard::SurakartaSimpleBoard(const SurakartaBoard& board) {//构造函数，用于从SurakartaBoard对象创建一个简化的棋盘状态。
    board_size_ = board.board_size_;

    for (auto line : board) {
        std::vector<PieceColor> simple_line;
        for (auto column : line) {
            PieceColor color = column->GetColor();
            simple_line.push_back(color);
        }
        data_.push_back(simple_line);
    }
}


void SurakartaSimpleBoard::UnsafeMove(const SurakartaMove& move) {//在简化的棋盘上执行一个移动，不进行合法性检查。
    PieceColor color = move.player;
    data_[move.from.x][move.from.y] = PieceColor::NONE;
    data_[move.to.x][move.to.y] = color;
}


SurakartaGame SurakartaSimpleBoard::AsNewGame(SurakartaPlayer current_player) const {//根据当前的简化棋盘状态创建一个新的SurakartaGame对象。
    SurakartaGame game(board_size_, 20);
    game.StartGame();

    for (unsigned int i = 0; i < board_size_; i++) {
        for (unsigned int j = 0; j < board_size_; j++) {
            PieceColor color = data_[i][j];
            (*game.board_)[i][j]->SetColor(color);
        }
    }

    game.GetGameInfo()->current_player_ = current_player;

    return game;
}


std::vector<SurakartaMove> SurakartaSimpleBoard::GetAllLegalMoves(SurakartaPlayer current_player) const {//获取当前玩家所有合法的移动。
    SurakartaGame game = AsNewGame(current_player);

    std::vector<SurakartaPosition> from_list;
    std::vector<SurakartaPosition> to_list;

    for (unsigned int i = 0; i < board_size_; i++) {
        for (unsigned int j = 0; j < board_size_; j++) {
            SurakartaPosition pos = {i, j};
            if (data_[i][j] == current_player) {
                from_list.push_back(pos);
            }
            else {
                to_list.push_back(pos);
            }
        }
    }

    std::vector<SurakartaMove> capture_move_list, non_capture_move_list;
    for (auto& from : from_list) {
        for (auto& to : to_list) {
            SurakartaMove move = {from, to, current_player};
            SurakartaIllegalMoveReason reason = game.GetRuleManager()->JudgeMove(move);
            if (reason == SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE) {
                capture_move_list.push_back(move);
            }
            else if (reason == SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE) {
                non_capture_move_list.push_back(move);
            }
        }
    }

    if (capture_move_list.size() > 0) {
        return capture_move_list;
    }
    else {
        return non_capture_move_list;
    }
}


bool SurakartaSimpleBoard::IsEnd() const {//判断游戏是否结束，即是否有一方的棋子全部被吃掉。
    int black = 0, white = 0;
    for (unsigned int i = 0; i < board_size_; i++) {
        for (unsigned int j = 0; j < board_size_; j++) {
            if (data_[i][j] == PieceColor::BLACK) {
                black++;
            }
            else if (data_[i][j] == PieceColor::WHITE) {
                white++;
            }
        }
    }
    return black == 0 || white == 0;
}


SurakartaPlayer SurakartaSimpleBoard::RunRandomGame(SurakartaPlayer current_player) const {//使用随机AI来运行一局游戏，返回游戏的胜者。
    SurakartaGame game = AsNewGame(current_player);
    auto rand_agent = std::make_shared<SurakartaAgentRandom>(game.GetBoard(), game.GetGameInfo(), game.GetRuleManager());

    while (!game.IsEnd()) {
        auto move = rand_agent->CalculateMove();
        game.Move(move);
    }
    // if (game.GetGameInfo()->end_reason_ == SurakartaEndReason::ILLIGAL_MOVE) {
    //     std::cout << current_player << std::endl;
    //     std::cout << game.GetGameInfo()->num_round_ << std::endl;
    //     std::cout << *game.GetBoard() << std::endl;
    // }

    return game.GetGameInfo()->Winner();
}
