#include "surakarta_rule_manager.h"
#include <iostream>

void SurakartaRuleManager::OnUpdateBoard() {
    // TODO:
    // Every time the board and game_info is updated to the next round version, this function will be called.
    // You don't need to implement this function if you don't need it.
    // A more delicate way is to use Qt's signal and slot mechanism, but that's the advanced part.
}

SurakartaIllegalMoveReason SurakartaRuleManager::JudgeMove(const SurakartaMove& move) {
    // TODO: Implement this function.
    // An example of how to get the color of a piece and the current player.
    PieceColor color_from = (*board_)[move.from.x][move.from.y]->GetColor();
    PieceColor color_to = (*board_)[move.to.x][move.to.y]->GetColor();
    if (move.from.x >= board_size_ || move.from.y >= board_size_ || move.to.x >= board_size_ || move.to.y >= board_size_) {
        return SurakartaIllegalMoveReason::OUT_OF_BOARD;  // 判断移动是否超出棋盘边界
    }
    if (color_from != PieceColor::BLACK && color_from != PieceColor::WHITE) {
        return SurakartaIllegalMoveReason::NOT_PIECE;  // 判断是否操作非棋子的位置
    }
    if (move.player != game_info_->current_player_) {
        return SurakartaIllegalMoveReason::NOT_PLAYER_TURN;  // 判断是否是合法玩家在操作
    }
    if (color_from != game_info_->current_player_) {
        return SurakartaIllegalMoveReason::NOT_PLAYER_PIECE;  // 判断玩家是否正确移动己方棋子
    }
    int x1 = move.from.x;
    int y1 = move.from.y;
    int x2 = move.to.x;
    int y2 = move.to.y;
    if (((abs(x1 - x2) == 1 && move.from.y == move.to.y) || (abs(y1 - y2) == 1 && move.from.x == move.to.x) || (abs(x1 - x2) == 1 && abs(y1 - y2) == 1)) && color_to == PieceColor::NONE) {
        return SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE;  // 判断是否是合法的不吃子移动
    } else if (color_to != PieceColor::NONE && color_to != game_info_->current_player_) {
        std::pair<int, int> direction;  // 向左：direction==1；向右：direction==2；向上：direction==3；向下：direction==4
        int flag1 = 0;                  // 逆时针路径是否有棋子阻挡的记录
        int flag2 = 0;                  // 顺时针路径是否有棋子阻挡的记录
        unsigned int i = move.from.x;   // 归位
        unsigned int j = move.from.y;   // 归位
        int times1 = 0;                 // 逆时针路径绕环的次数
        int times2 = 0;                 // 顺时针路径绕环的次数
        SurakartaIllegalMoveReason reason = SurakartaIllegalMoveReason::ILLIGAL_CAPTURE_MOVE;
        for (unsigned int k = 1; k <= board_size_ / 2 - 1; k++) {  // n*n的棋盘，一共有n/2-1条环路，为了防止比如6*6棋盘中（2，2）类位置的影响，我们讨论move.from在不会互相干扰的行或列上
            if ((move.from.x == k || move.from.x == board_size_ - 1 - k) && (move.to.x == k || move.to.y == k || move.to.x == board_size_ - 1 - k || move.to.y == board_size_ - 1 - k)) {
                flag1 = 0;  // 每次都归位
                flag2 = 0;
                i = move.from.x;
                j = move.from.y;
                times1 = 0;
                times2 = 0;
                if (move.from.x == k) {
                    direction.first = 1;   // 逆时针的起步方向
                    direction.second = 2;  // 顺时针的起步方向
                } else {
                    direction.first = 2;
                    direction.second = 1;
                }
                while (i != move.to.x || j != move.to.y) {  // 模拟棋子的移动，逆时针
                    if (i == k && direction.first == 1) {
                        if (j > 0) {
                            j--;
                        } else if (j == 0) {
                            i = 0;
                            j = k;
                            direction.first = 4;
                            times1++;  // 绕环，次数++
                        }
                    } else if (j == k && direction.first == 4) {
                        if (i < board_size_ - 1) {
                            i++;
                        } else if (i == board_size_ - 1) {
                            i = board_size_ - 1 - k;
                            j = 0;
                            direction.first = 2;
                            times1++;
                        }
                    } else if (i == board_size_ - 1 - k && direction.first == 2) {
                        if (j < board_size_ - 1) {
                            j++;
                        } else if (j == board_size_ - 1) {
                            i = board_size_ - 1;
                            j = board_size_ - 1 - k;
                            direction.first = 3;
                            times1++;
                        }
                    } else if (j == board_size_ - 1 - k && direction.first == 3) {
                        if (i > 0) {
                            i--;
                        } else if (i == 0) {
                            i = k;
                            j = board_size_ - 1;
                            direction.first = 1;
                            times1++;
                        }
                    }
                    if ((*board_)[i][j]->GetColor() != PieceColor::NONE) {  // 判断是否遇到障碍
                        if (i != move.to.x || j != move.to.y) {             // 遇到move.to要排除
                            if (i != move.from.x || j != move.from.y) {     // move.from位置已经空了，也要排除
                                flag1 = 1;
                                break;
                            }
                        }
                    }
                }
                i = move.from.x;  // 归位
                j = move.from.y;
                while (i != move.to.x || j != move.to.y) {  // 模拟棋子的移动，顺时针
                    if (i == k && direction.second == 2) {
                        if (j < board_size_ - 1) {
                            j++;
                        } else if (j == board_size_ - 1) {
                            i = 0;
                            j = board_size_ - 1 - k;
                            direction.second = 4;
                            times2++;
                        }
                    } else if (j == board_size_ - 1 - k && direction.second == 4) {
                        if (i < board_size_ - 1) {
                            i++;
                        } else if (i == board_size_ - 1) {
                            i = board_size_ - 1 - k;
                            j = board_size_ - 1;
                            direction.second = 1;
                            times2++;
                        }
                    } else if (i == board_size_ - 1 - k && direction.second == 1) {
                        if (j > 0) {
                            j--;
                        } else if (j == 0) {
                            i = board_size_ - 1;
                            j = k;
                            direction.second = 3;
                            times2++;
                        }
                    } else if (j == k && direction.second == 3) {
                        if (i > 0) {
                            i--;
                        } else if (i == 0) {
                            i = k;
                            j = 0;
                            direction.second = 2;
                            times2++;
                        }
                    }
                    if ((*board_)[i][j]->GetColor() != PieceColor::NONE) {
                        if (i != move.to.x || j != move.to.y) {
                            if (i != move.from.x || j != move.from.y) {
                                flag2 = 1;
                                break;
                            }
                        }
                    }
                }
                if ((flag2 == 0 && times2 > 0) || (flag1 == 0 && times1 > 0)) {  // 没有遇到障碍且绕过至少一个环，就是合法的
                    reason = SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
                }
            }
            if ((move.from.y == k || move.from.y == board_size_ - 1 - k) && (move.to.x == k || move.to.y == k || move.to.x == board_size_ - 1 - k || move.to.y == board_size_ - 1 - k)) {
                flag1 = 0;
                flag2 = 0;
                i = move.from.x;
                j = move.from.y;
                times1 = 0;
                times2 = 0;
                if (move.from.y == k) {
                    direction.first = 4;
                    direction.second = 3;
                } else {
                    direction.first = 3;
                    direction.second = 4;
                }
                while (i != move.to.x || j != move.to.y) {
                    if (i == k && direction.first == 1) {
                        if (j > 0) {
                            j--;
                        } else if (j == 0) {
                            i = 0;
                            j = k;
                            direction.first = 4;
                            times1++;
                        }
                    } else if (j == k && direction.first == 4) {
                        if (i < board_size_ - 1) {
                            i++;
                        } else if (i == board_size_ - 1) {
                            i = board_size_ - 1 - k;
                            j = 0;
                            direction.first = 2;
                            times1++;
                        }
                    } else if (i == board_size_ - 1 - k && direction.first == 2) {
                        if (j < board_size_ - 1) {
                            j++;
                        } else if (j == board_size_ - 1) {
                            i = board_size_ - 1;
                            j = board_size_ - 1 - k;
                            direction.first = 3;
                            times1++;
                        }
                    } else if (j == board_size_ - 1 - k && direction.first == 3) {
                        if (i > 0) {
                            i--;
                        } else if (i == 0) {
                            i = k;
                            j = board_size_ - 1;
                            direction.first = 1;
                            times1++;
                        }
                    }
                    if ((*board_)[i][j]->GetColor() != PieceColor::NONE) {
                        if (i != move.to.x || j != move.to.y) {
                            if (i != move.from.x || j != move.from.y) {
                                flag1 = 1;
                                break;
                            }
                        }
                    }
                }
                i = move.from.x;
                j = move.from.y;
                while (i != move.to.x || j != move.to.y) {
                    if (i == k && direction.second == 2) {
                        if (j < board_size_ - 1) {
                            j++;
                        } else if (j == board_size_ - 1) {
                            i = 0;
                            j = board_size_ - 1 - k;
                            direction.second = 4;
                            times2++;
                        }
                    } else if (j == board_size_ - 1 - k && direction.second == 4) {
                        if (i < board_size_ - 1) {
                            i++;
                        } else if (i == board_size_ - 1) {
                            i = board_size_ - 1 - k;
                            j = board_size_ - 1;
                            direction.second = 1;
                            times2++;
                        }
                    } else if (i == board_size_ - 1 - k && direction.second == 1) {
                        if (j > 0) {
                            j--;
                        } else if (j == 0) {
                            i = board_size_ - 1;
                            j = k;
                            direction.second = 3;
                            times2++;
                        }
                    } else if (j == k && direction.second == 3) {
                        if (i > 0) {
                            i--;
                        } else if (i == 0) {
                            i = k;
                            j = 0;
                            direction.second = 2;
                            times2++;
                        }
                    }
                    if ((*board_)[i][j]->GetColor() != PieceColor::NONE) {
                        if (i != move.to.x || j != move.to.y) {
                            if (i != move.from.x || j != move.from.y) {
                                flag2 = 1;
                                break;
                            }
                        }
                    }
                }
                if ((flag2 == 0 && times2 > 0) || (flag1 == 0 && times1 > 0)) {
                    reason = SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE;
                }
            }
        }
        return reason;
    } else {
        return SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE;
    }
}

std::pair<SurakartaEndReason, SurakartaPlayer> SurakartaRuleManager::JudgeEnd(const SurakartaIllegalMoveReason reason) {
    // TODO: Implement this function.
    // Note that at this point, the board and game_info have not been updated yet.
    SurakartaPlayer current_player = game_info_->current_player_;
    if (reason == SurakartaIllegalMoveReason::ILLIGAL || reason == SurakartaIllegalMoveReason::NOT_PLAYER_TURN || reason == SurakartaIllegalMoveReason::OUT_OF_BOARD || reason == SurakartaIllegalMoveReason::NOT_PIECE || reason == SurakartaIllegalMoveReason::NOT_PLAYER_PIECE || reason == SurakartaIllegalMoveReason::ILLIGAL_CAPTURE_MOVE || reason == SurakartaIllegalMoveReason::ILLIGAL_NON_CAPTURE_MOVE) {
        if (current_player == SurakartaPlayer::WHITE) {
            return std::make_pair(SurakartaEndReason::ILLIGAL_MOVE, SurakartaPlayer::BLACK);
        } else {
            return std::make_pair(SurakartaEndReason::ILLIGAL_MOVE, SurakartaPlayer::WHITE);
        }
    }
    int black = 0;
    int white = 0;
    for (unsigned int i = 0; i < board_size_; i++) {
        for (unsigned int j = 0; j < board_size_; j++) {
            PieceColor color = (*board_)[i][j]->GetColor();
            if (color == PieceColor::BLACK) {
                black++;
            } else if (color == PieceColor::WHITE) {
                white++;
            }
        }
    }
    if (black == 1 || white == 1) {  // 因为棋盘未更新，所以如果一方只剩一个棋子还可以被对手吃，并且不是自己回合，那么他必输
        if (black == 1 && reason == SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE && current_player == SurakartaPlayer::WHITE) {
            return std::make_pair(SurakartaEndReason::CHECKMATE, SurakartaPlayer::WHITE);
        } else if (white == 1 && reason == SurakartaIllegalMoveReason::LEGAL_CAPTURE_MOVE && current_player == SurakartaPlayer::BLACK) {
            return std::make_pair(SurakartaEndReason::CHECKMATE, SurakartaPlayer::BLACK);
        }
    }
    if (game_info_->num_round_ - game_info_->last_captured_round_ == game_info_->max_no_capture_round_ && reason == SurakartaIllegalMoveReason::LEGAL_NON_CAPTURE_MOVE) {  // 判断连续未吃子回合数是否超过限制
        if (black > white) {
            return std::make_pair(SurakartaEndReason::STALEMATE, SurakartaPlayer::BLACK);
        } else if (black < white) {
            return std::make_pair(SurakartaEndReason::STALEMATE, SurakartaPlayer::WHITE);
        } else {
            return std::make_pair(SurakartaEndReason::STALEMATE, SurakartaPlayer::NONE);
        }
    }
    return std::make_pair(SurakartaEndReason::NONE, SurakartaPlayer::NONE);
}

std::unique_ptr<std::vector<SurakartaPosition>> SurakartaRuleManager::GetAllLegalTarget([[maybe_unused]] const SurakartaPosition postion) {
    // TODO:
    // We don't test this function, you don't need to implement this function if you don't need it.
    return std::make_unique<std::vector<SurakartaPosition>>();
}

void SurakartaRuleManager::HelloWorld() {
    std::cout << "Hello World!" << std::endl;
}

