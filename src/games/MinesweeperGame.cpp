#include "MinesweeperGame.h"
#include <QGridLayout>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QDebug>
#include <QMouseEvent>
#include <cmath>
#include <random>
#include <algorithm>

namespace SA {

MinesweeperGame::MinesweeperGame(QWidget* parent) : MiniGame(parent) {
    auto* layout = new QVBoxLayout(this);
    auto* title = new QLabel(displayName(), this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #8B1A1A;");

    statusLabel_ = new QLabel(this);

    statusLabel_->setStyleSheet(
        "font-size:14px;"
        "color:#3B5BA5;"
        );

    grid_ = new QGridLayout;
    grid_->setSpacing(2);

    layout->addWidget(title);
    layout->addWidget(statusLabel_);
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
    statusLabel_->setText(
        tr("游戏开始")
        );

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

    buttons_.assign(
        rows_,
        std::vector<QPushButton*>(cols_, nullptr)
        );

    for(int r = 0; r < rows_; ++r) {

        for(int c = 0; c < cols_; ++c) {

            auto* btn = new QPushButton(this);

            btn->setFixedSize(30, 30);

            btn->setStyleSheet(
                "background-color:#C0C0C0;"
                );

            btn->installEventFilter(this);

            const int row = r;
            const int col = c;

            connect(btn,
                    &QPushButton::clicked,
                    this,
                    [this,row,col]() {

                        onCellClicked(row,col);
                    });

            grid_->addWidget(btn,r,c);

            buttons_[r][c] = btn;
        }
    }
}

bool MinesweeperGame::eventFilter(QObject* obj, QEvent* event)
{
    if(event->type() == QEvent::MouseButtonPress) {

        auto* mouseEvent =
            static_cast<QMouseEvent*>(event);

        if(mouseEvent->button() == Qt::RightButton) {

            for(int r = 0; r < rows_; ++r) {

                for(int c = 0; c < cols_; ++c) {

                    if(buttons_[r][c] == obj) {

                        onCellRightClicked(r,c);

                        return true;
                    }
                }
            }
        }
    }

    return QWidget::eventFilter(obj,event);
}

void MinesweeperGame::onCellClicked(int row, int col) {
    if (gameEnded_ || board_[row][col].isRevealed || board_[row][col].isFlagged) return;

    if (firstClick_) {
        placeMines(row, col);
        calculateAdjacent();
        firstClick_ = false;
    }

    if(board_[row][col].isMine) {

        gameEnded_ = true;

        revealAllMines();

        statusLabel_->setText(
            tr("踩到地雷，游戏失败")
            );

        QMessageBox::information(
            this,
            tr("失败"),
            tr("你踩到地雷了！")
            );

        emit stressOccurred(8);

        emit finished(0,false);

        return;
    }

    revealCell(row, col);

    if(checkWin()) {

        gameEnded_ = true;

        int score =
            std::max(
                50,
                150 - revealedCount_
                );

        statusLabel_->setText(
            tr("成功完成扫雷！")
            );

        QMessageBox::information(
            this,
            tr("胜利"),
            tr("恭喜完成扫雷！\n获得分数：%1")
                .arg(score)
            );

        emit finished(score,true);
    }
}

void MinesweeperGame::onCellRightClicked(int row, int col)
{
    if(gameEnded_) return;

    auto& cell = board_[row][col];

    if(cell.isRevealed) return;

    cell.isFlagged = !cell.isFlagged;

    if(cell.isFlagged) {

        buttons_[row][col]->setText("🚩");

    } else {

        buttons_[row][col]->setText("");
    }
}

void MinesweeperGame::placeMines(int safeRow, int safeCol)
{
    std::vector<std::pair<int,int>> candidates;

    for(int r = 0; r < rows_; ++r) {

        for(int c = 0; c < cols_; ++c) {

            bool safe =
                std::abs(r - safeRow) <= 1
                &&
                std::abs(c - safeCol) <= 1;

            if(!safe) {

                candidates.push_back({r,c});
            }
        }
    }

    static std::random_device rd;

    static std::mt19937 gen(rd());

    std::shuffle(
        candidates.begin(),
        candidates.end(),
        gen
        );

    for(int i = 0; i < totalMines_; ++i) {

        int r = candidates[i].first;

        int c = candidates[i].second;

        board_[r][c].isMine = true;
    }
}

void MinesweeperGame::calculateAdjacent()
{
    for(int r = 0; r < rows_; ++r) {

        for(int c = 0; c < cols_; ++c) {

            if(board_[r][c].isMine)
                continue;

            int cnt = 0;

            for(int dr = -1; dr <= 1; ++dr) {

                for(int dc = -1; dc <= 1; ++dc) {

                    int nr = r + dr;

                    int nc = c + dc;

                    if(nr < 0 || nr >= rows_
                        || nc < 0 || nc >= cols_)
                        continue;

                    if(board_[nr][nc].isMine)
                        cnt++;
                }
            }

            board_[r][c].adjacentMines = cnt;
        }
    }
}

void MinesweeperGame::revealCell(int row, int col)
{
    if(row < 0 || row >= rows_
        || col < 0 || col >= cols_)
        return;

    auto& cell = board_[row][col];

    if(cell.isRevealed || cell.isFlagged)
        return;

    cell.isRevealed = true;

    revealedCount_++;

    updateCellAppearance(row,col);

            // 有数字就停止展开
    if(cell.adjacentMines > 0)
        return;

            // 自动展开周围
    for(int dr = -1; dr <= 1; ++dr) {

        for(int dc = -1; dc <= 1; ++dc) {

            if(dr == 0 && dc == 0)
                continue;

            revealCell(
                row + dr,
                col + dc
                );
        }
    }
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

void MinesweeperGame::updateCellAppearance(int row, int col)
{
    auto* btn = buttons_[row][col];

    auto& cell = board_[row][col];

    btn->setEnabled(false);

    btn->setStyleSheet(
        "background-color:#E0E0E0;"
        );

    if(cell.isMine) {

        btn->setText("💣");

        return;
    }

    if(cell.adjacentMines > 0) {

        btn->setText(
            QString::number(
                cell.adjacentMines
                )
            );
    }
}

bool MinesweeperGame::checkWin() const {
    return revealedCount_ == rows_ * cols_ - totalMines_;
}

} // namespace SA
