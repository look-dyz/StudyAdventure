#include "TicTacToeGame.h"
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QDebug>
#include <random>
#include <climits>

namespace SA {

TicTacToeGame::TicTacToeGame(QWidget* parent) : MiniGame(parent) {
    buildUI();
}

void TicTacToeGame::buildUI() {
    auto* layout = new QVBoxLayout(this);

    auto* title = new QLabel(displayName(), this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #8B1A1A;");

    statusLabel_ = new QLabel(tr("你的回合（X）"), this);
    statusLabel_->setAlignment(Qt::AlignCenter);
    statusLabel_->setStyleSheet("font-size: 16px; color: #3B5BA5;");

    grid_ = new QGridLayout;
    grid_->setSpacing(4);
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            auto* btn = new QPushButton(this);
            btn->setFixedSize(80, 80);
            btn->setStyleSheet("font-size: 36px; font-weight: bold;");
            const int row = r, col = c;
            connect(btn, &QPushButton::clicked, this, [this, row, col]() {
                onCellClicked(row, col);
            });
            grid_->addWidget(btn, r, c);
            buttons_[r][c] = btn;
        }
    }

    layout->addWidget(title);
    layout->addWidget(statusLabel_);
    layout->addLayout(grid_);
    layout->addStretch();
}

void TicTacToeGame::start() {
    reset();
}

void TicTacToeGame::reset() {
    for (auto& row : board_) row.fill(Mark::Empty);
    playerTurn_ = true;
    gameEnded_ = false;
    statusLabel_->setText(tr("你的回合（X）"));
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            buttons_[r][c]->setText("");
            buttons_[r][c]->setEnabled(true);
        }
    }
}

void TicTacToeGame::onCellClicked(int row, int col) {
    if (gameEnded_ || !playerTurn_) return;
    if (board_[row][col] != Mark::Empty) return;

    board_[row][col] = Mark::X;
    updateCell(row, col);

    Mark winner = checkWinner(board_);
    if (winner != Mark::Empty || isBoardFull(board_)) {
        endGame(winner);
        return;
    }

    playerTurn_ = false;
    statusLabel_->setText(tr("AI 思考中..."));
    // 延迟一点让玩家有时间看清
    QTimer::singleShot(400, this, [this]() { aiMove(); });
}

void TicTacToeGame::aiMove() {
    std::pair<int,int> move;
    switch (difficulty_) {
        case Difficulty::Easy:   move = randomMove(); break;
        case Difficulty::Normal: move = bestMove(); break;
        case Difficulty::Hard:   move = bestMove(); break;  // 普通和困难都用 Minimax，差别在剪枝
    }

    board_[move.first][move.second] = Mark::O;
    updateCell(move.first, move.second);

    Mark winner = checkWinner(board_);
    if (winner != Mark::Empty || isBoardFull(board_)) {
        endGame(winner);
        return;
    }

    playerTurn_ = true;
    statusLabel_->setText(tr("你的回合（X）"));
}

std::pair<int,int> TicTacToeGame::randomMove() {
    std::vector<std::pair<int,int>> empties;
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            if (board_[r][c] == Mark::Empty) empties.push_back({r, c});

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, empties.size() - 1);
    return empties[dist(gen)];
}

std::pair<int,int> TicTacToeGame::bestMove() {
    // TODO（成员 C）：完整实现 Minimax 寻找最佳走法
    // 当前为占位实现：返回第一个空位
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 3; ++c) {
            if (board_[r][c] == Mark::Empty) {
                Board b = board_;
                b[r][c] = Mark::O;
                int score = minimax(b, 0, false, INT_MIN, INT_MAX);
                qDebug() << "[TicTacToe] try" << r << c << "score=" << score;
                // TODO：选择 score 最大的走法
                return {r, c};
            }
        }
    }
    return {0, 0};
}

int TicTacToeGame::minimax(Board& b, int depth, bool isMax, int alpha, int beta) {
    // TODO（成员 C）：完整的 Minimax + α-β 剪枝
    // 终止条件：检查胜负或棋盘已满
    // O 想 max，X 想 min
    Q_UNUSED(b);
    Q_UNUSED(depth);
    Q_UNUSED(isMax);
    Q_UNUSED(alpha);
    Q_UNUSED(beta);
    return 0;
}

TicTacToeGame::Mark TicTacToeGame::checkWinner(const Board& b) const {
    // 行
    for (int r = 0; r < 3; ++r) {
        if (b[r][0] != Mark::Empty && b[r][0] == b[r][1] && b[r][1] == b[r][2])
            return b[r][0];
    }
    // 列
    for (int c = 0; c < 3; ++c) {
        if (b[0][c] != Mark::Empty && b[0][c] == b[1][c] && b[1][c] == b[2][c])
            return b[0][c];
    }
    // 对角线
    if (b[0][0] != Mark::Empty && b[0][0] == b[1][1] && b[1][1] == b[2][2]) return b[0][0];
    if (b[0][2] != Mark::Empty && b[0][2] == b[1][1] && b[1][1] == b[2][0]) return b[0][2];
    return Mark::Empty;
}

bool TicTacToeGame::isBoardFull(const Board& b) const {
    for (const auto& row : b)
        for (auto m : row)
            if (m == Mark::Empty) return false;
    return true;
}

void TicTacToeGame::updateCell(int row, int col) {
    auto* btn = buttons_[row][col];
    btn->setText(board_[row][col] == Mark::X ? "X" : "O");
    btn->setEnabled(false);
}

void TicTacToeGame::endGame(Mark winner) {
    gameEnded_ = true;
    for (auto& row : buttons_)
        for (auto* btn : row)
            btn->setEnabled(false);

    bool playerWon = (winner == Mark::X);
    bool draw = (winner == Mark::Empty);

    int score;
    QString msg;
    if (playerWon) {
        score = 80;
        msg = tr("你赢了！");
    } else if (draw) {
        score = 50;
        msg = tr("平局");
    } else {
        score = 10;
        msg = tr("AI 赢了");
    }
    statusLabel_->setText(msg);
    emit finished(score, playerWon || draw);
}

} // namespace SA
