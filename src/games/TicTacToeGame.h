/**
 * @file TicTacToeGame.h
 * @brief 井字棋小游戏（对应 AI 引论学科）
 *
 * 玩法：玩家 X vs AI O，3 种 AI 难度。
 * AI 重点：Minimax 算法 + α-β 剪枝。
 *
 * 负责人：成员 C   预计完成：5/22
 */
#ifndef STUDYADVENTURE_TICTACTOEGAME_H
#define STUDYADVENTURE_TICTACTOEGAME_H

#include "common/MiniGame.h"
#include <array>
#include <utility>

class QGridLayout;
class QPushButton;
class QLabel;

namespace SA {

class TicTacToeGame : public MiniGame {
    Q_OBJECT
public:
    explicit TicTacToeGame(QWidget* parent = nullptr);
    ~TicTacToeGame() override = default;

    void start() override;
    void reset() override;
    MiniGameType type() const override { return MiniGameType::TicTacToe; }
    QString displayName() const override { return tr("井字棋"); }

private slots:
    void onCellClicked(int row, int col);

private:
    enum class Mark { Empty, X, O };
    using Board = std::array<std::array<Mark, 3>, 3>;

    Board board_;
    bool playerTurn_ = true;
    bool gameEnded_ = false;

    QGridLayout* grid_ = nullptr;
    std::array<std::array<QPushButton*, 3>, 3> buttons_;
    QLabel* statusLabel_ = nullptr;

    void buildUI();
    void aiMove();                          ///< AI 落子
    std::pair<int,int> randomMove();         ///< 简单难度：随机
    std::pair<int,int> bestMove();           ///< 普通/困难：Minimax
    int minimax(Board& b, int depth, bool isMax, int alpha, int beta);
    Mark checkWinner(const Board& b) const;
    bool isBoardFull(const Board& b) const;
    void updateCell(int row, int col);
    void endGame(Mark winner);
};

} // namespace SA

#endif // STUDYADVENTURE_TICTACTOEGAME_H
