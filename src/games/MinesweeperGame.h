/**
 * @file MinesweeperGame.h
 * @brief 扫雷小游戏（对应程设学科）
 *
 * 玩法：经典扫雷规则，分简单/普通/困难三档。
 * 算法重点：递归展开 0 格、首次点击保护机制。
 *
 * 负责人：成员 C   预计完成：5/19
 */
#ifndef STUDYADVENTURE_MINESWEEPERGAME_H
#define STUDYADVENTURE_MINESWEEPERGAME_H

#include "common/MiniGame.h"
#include <vector>
#include <QEvent>

class QGridLayout;
class QPushButton;
class QLabel;

namespace SA {

class MinesweeperGame : public MiniGame {
    Q_OBJECT
public:
    explicit MinesweeperGame(QWidget* parent = nullptr);
    ~MinesweeperGame() override = default;

    void start() override;
    void reset() override;
    MiniGameType type() const override { return MiniGameType::Minesweeper; }
    QString displayName() const override { return tr("扫雷"); }

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private slots:
    void onCellClicked(int row, int col);
    void onCellRightClicked(int row, int col);

private:
    struct Cell {
        bool isMine = false;
        bool isRevealed = false;
        bool isFlagged = false;
        int adjacentMines = 0;
    };

    int rows_ = 9;
    int cols_ = 9;
    int totalMines_ = 10;
    int revealedCount_ = 0;
    bool firstClick_ = true;
    bool gameEnded_ = false;

    std::vector<std::vector<Cell>> board_;
    std::vector<std::vector<QPushButton*>> buttons_;
    QGridLayout* grid_ = nullptr;
    QLabel* statusLabel_ = nullptr;

    void buildBoard();          ///< 构建棋盘 UI
    void placeMines(int safeRow, int safeCol);  ///< 首次点击后布雷
    void calculateAdjacent();
    void revealCell(int row, int col);           ///< 递归展开 0 格
    void revealAllMines();
    void updateCellAppearance(int row, int col);
    bool checkWin() const;
    void applyDifficulty();
};

} // namespace SA

#endif // STUDYADVENTURE_MINESWEEPERGAME_H
