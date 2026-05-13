#include "MinesweeperGame.h"
#include <QGridLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QDebug>

namespace SA {

MinesweeperGame::MinesweeperGame(QWidget* parent) : MiniGame(parent) {
    auto* layout = new QVBoxLayout(this);
    auto* title = new QLabel(displayName(), this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #8B1A1A;");

    grid_ = new QGridLayout;
    grid_->setSpacing(2);

    layout->addWidget(title);
    layout->addLayout(grid_);
    layout->addStretch();
}

void MinesweeperGame::start() {
    applyDifficulty();
    reset();
}

void MinesweeperGame::reset() {
    // 清理旧 UI
    for (auto& row : buttons_) {
        for (auto* btn : row) {
            grid_->removeWidget(btn);
            btn->deleteLater();
        }
    }
    buttons_.clear();
    board_.clear();
    revealedCount_ = 0;
    firstClick_ = true;
    gameEnded_ = false;

    // 初始化棋盘数据
    board_.assign(rows_, std::vector<Cell>(cols_));
    buildBoard();
}

void MinesweeperGame::applyDifficulty() {
    switch (difficulty_) {
        case Difficulty::Easy:   rows_ = 9;  cols_ = 9;  totalMines_ = 10; break;
        case Difficulty::Normal: rows_ = 16; cols_ = 16; totalMines_ = 40; break;
        case Difficulty::Hard:   rows_ = 22; cols_ = 22; totalMines_ = 99; break;
    }
}

void MinesweeperGame::buildBoard() {
    buttons_.assign(rows_, std::vector<QPushButton*>(cols_, nullptr));
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            auto* btn = new QPushButton(this);
            btn->setFixedSize(30, 30);
            btn->setStyleSheet("background-color: #C0C0C0;");
            // TODO（成员 C）：左键正常 clicked、右键需要 eventFilter 或派生 QPushButton 处理
            const int row = r, col = c;
            connect(btn, &QPushButton::clicked, this, [this, row, col]() {
                onCellClicked(row, col);
            });
            grid_->addWidget(btn, r, c);
            buttons_[r][c] = btn;
        }
    }
}

void MinesweeperGame::onCellClicked(int row, int col) {
    if (gameEnded_ || board_[row][col].isRevealed || board_[row][col].isFlagged) return;

    if (firstClick_) {
        placeMines(row, col);
        calculateAdjacent();
        firstClick_ = false;
    }

    if (board_[row][col].isMine) {
        gameEnded_ = true;
        revealAllMines();
        emit stressOccurred(8);
        emit finished(0, false);
        return;
    }

    revealCell(row, col);

    if (checkWin()) {
        gameEnded_ = true;
        int score = std::max(50, 100 - revealedCount_);  // 占位评分
        emit finished(score, true);
    }
}

void MinesweeperGame::onCellRightClicked(int row, int col) {
    // TODO（成员 C）：实现旗子标记
    Q_UNUSED(row);
    Q_UNUSED(col);
}

void MinesweeperGame::placeMines(int safeRow, int safeCol) {
    // TODO（成员 C）：在排除 (safeRow, safeCol) 及其 8 邻居后随机布雷
    // 推荐用 std::random_device + std::mt19937 + std::shuffle
    Q_UNUSED(safeRow);
    Q_UNUSED(safeCol);
    qDebug() << "[Minesweeper] placeMines (not implemented)";
}

void MinesweeperGame::calculateAdjacent() {
    // TODO（成员 C）：对每个非雷格，统计 8 邻居中的雷数
    qDebug() << "[Minesweeper] calculateAdjacent (not implemented)";
}

void MinesweeperGame::revealCell(int row, int col) {
    // TODO（成员 C）：递归或 BFS 展开。若 adjacentMines == 0，自动展开 8 邻居
    if (row < 0 || row >= rows_ || col < 0 || col >= cols_) return;
    if (board_[row][col].isRevealed) return;
    board_[row][col].isRevealed = true;
    revealedCount_++;
    updateCellAppearance(row, col);
}

void MinesweeperGame::revealAllMines() {
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            if (board_[r][c].isMine) {
                buttons_[r][c]->setText("💣");
            }
        }
    }
}

void MinesweeperGame::updateCellAppearance(int row, int col) {
    auto* btn = buttons_[row][col];
    btn->setEnabled(false);
    if (board_[row][col].adjacentMines > 0) {
        btn->setText(QString::number(board_[row][col].adjacentMines));
    }
    btn->setStyleSheet("background-color: #E0E0E0;");
}

bool MinesweeperGame::checkWin() const {
    return revealedCount_ == rows_ * cols_ - totalMines_;
}

} // namespace SA
