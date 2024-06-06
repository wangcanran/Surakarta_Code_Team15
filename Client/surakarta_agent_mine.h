#pragma once

#include <memory>
#include "surakarta_game.h"
#include "surakarta_common.h"
#include "surakarta_agent_base.h"


class SurakartaAgentMine : public SurakartaAgentBase {
public:
    using SurakartaAgentBase::SurakartaAgentBase;
    SurakartaMove CalculateMove() override;

    // getters
    std::shared_ptr<const SurakartaBoard> GetBoardPtr() const { return board_; }
    unsigned int GetBoardSize() const { return board_size_; }
    std::shared_ptr<const SurakartaGameInfo> GetGameInfo() const { return game_info_; }
    std::shared_ptr<SurakartaRuleManager> GetRuleManager() const { return rule_manager_; }
};


// Simple board, based on 2D vector of chess color
class SurakartaSimpleBoard {
public:
    std::vector<std::vector<PieceColor>> data_;
    unsigned int board_size_;

    SurakartaSimpleBoard(const SurakartaBoard& board);

    void UnsafeMove(const SurakartaMove& move);
    std::vector<SurakartaMove> GetAllLegalMoves(SurakartaPlayer current_player) const;
    bool IsEnd() const;
    SurakartaPlayer RunRandomGame(SurakartaPlayer current_player) const;

    friend inline std::ostream& operator<<(std::ostream& os, const SurakartaSimpleBoard& board) {
        for (unsigned int y = 0; y < board.board_size_; y++) {
            for (unsigned int x = 0; x < board.board_size_; x++) {
                os << board.data_[x][y] << " ";
            }
            os << std::endl;
        }
        return os;
    }

private:
    SurakartaGame AsNewGame(SurakartaPlayer current_player) const;
};


// Node for MCTS Algorithm
class MNode {
public:
    // Node info
    int visits_ = 0;
    double reward_ = 0.0;
    std::vector<MNode*> children_;
    MNode* parent_ = nullptr;

    // State info
    const std::unique_ptr<const SurakartaSimpleBoard> simple_board_; // Board state of this node, not current board
    const SurakartaPlayer player_;
    std::vector<SurakartaMove> legal_moves_;  // match corresponding children by index
    SurakartaMove parent_move_;  // parent.move(parent_move_) == this

    MNode(std::unique_ptr<const SurakartaSimpleBoard> simple_board, const SurakartaPlayer player);

    ~MNode() {
        for (auto child : children_) {
            delete child;
        }
    }

    // MCTS algorithm
    MNode* Select();
    MNode* Expand();
    SurakartaPlayer Simulate();
    void BackPropagate(SurakartaPlayer winner);

    // utils
    bool IsFullyExpanded();
    double UCB(double C);
    MNode* MaxUCBChild(double C);
};

