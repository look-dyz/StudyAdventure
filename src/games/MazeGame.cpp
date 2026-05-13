#include "MazeGame.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QLabel>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QDebug>
#include <queue>
#include <random>

namespace SA {

constexpr int CELL_SIZE = 28;

MazeGame::MazeGame(QWidget* parent) : MiniGame(parent) {
    auto* layout = new QVBoxLayout(this);

    auto* title = new QLabel(displayName(), this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #8B1A1A;");

    statusLabel_ = new QLabel(this);

    scene_ = new QGraphicsScene(this);
    view_ = new QGraphicsView(scene_, this);
    view_->setFocusPolicy(Qt::StrongFocus);

    layout->addWidget(title);
    layout->addWidget(statusLabel_);
    layout->addWidget(view_);
    layout->addStretch();

    setFocusPolicy(Qt::StrongFocus);
}

void MazeGame::start() {
    // 按难度调整步数限制
    switch (difficulty_) {
        case Difficulty::Easy:   stepsRemaining_ = 60; break;
        case Difficulty::Normal: stepsRemaining_ = 45; break;
        case Difficulty::Hard:   stepsRemaining_ = 30; break;
    }
    reset();
}

void MazeGame::reset() {
    maze_.assign(rows_, std::vector<CellType>(cols_, CellType::Wall));
    hintsUsed_ = 0;
    gameEnded_ = false;
    generateMaze();
    placeSpecialCells();

    playerRow_ = 0;
    playerCol_ = 0;
    endRow_ = rows_ - 1;
    endCol_ = cols_ - 1;
    maze_[playerRow_][playerCol_] = CellType::Start;
    maze_[endRow_][endCol_] = CellType::End;

    redraw();
    statusLabel_->setText(tr("剩余步数 %1 | 按 H 显示 AI 提示（%2/%3）")
                          .arg(stepsRemaining_).arg(maxHints_ - hintsUsed_).arg(maxHints_));
    setFocus();
}

void MazeGame::generateMaze() {
    // TODO（成员 C）：递归回溯算法生成迷宫
    // 起点 (0,0)，调用 carve(0, 0)
    carve(0, 0);
}

void MazeGame::carve(int r, int c) {
    // TODO（成员 C）：DFS 挖通路实现
    // 1. 标记当前 (r, c) 为 Empty
    // 2. 随机打乱 4 个方向
    // 3. 对每个方向 (dr, dc)，如果 (r+2*dr, c+2*dc) 仍是 Wall 且在界内：
    //    a. 挖通 (r+dr, c+dc)
    //    b. 递归 carve(r+2*dr, c+2*dc)
    maze_[r][c] = CellType::Empty;
    // 占位：暂时把所有格子都设为 Empty 让游戏能跑
    for (auto& row : maze_) {
        for (auto& cell : row) {
            if (cell == CellType::Wall) cell = CellType::Empty;
        }
    }
}

void MazeGame::placeSpecialCells() {
    // TODO（成员 C）：随机放置陷阱格和知识格
    // 注意：不要覆盖起点和终点
}

std::vector<std::pair<int,int>> MazeGame::bfsShortestPath() {
    // TODO（成员 C）：完整 BFS 实现，返回从当前玩家位置到终点的最短路径
    // 提示：用 std::queue<std::pair<int,int>>，用 prev[][] 记录前驱
    std::vector<std::pair<int,int>> path;
    return path;
}

void MazeGame::redraw() {
    scene_->clear();
    for (int r = 0; r < rows_; ++r) {
        for (int c = 0; c < cols_; ++c) {
            QColor color;
            switch (maze_[r][c]) {
                case CellType::Wall:      color = QColor("#333333"); break;
                case CellType::Empty:     color = QColor("#EEEEEE"); break;
                case CellType::Trap:      color = QColor("#FF6B6B"); break;
                case CellType::Knowledge: color = QColor("#FFD93D"); break;
                case CellType::Start:     color = QColor("#6BCB77"); break;
                case CellType::End:       color = QColor("#FF6B9D"); break;
            }
            auto* rect = scene_->addRect(c * CELL_SIZE, r * CELL_SIZE,
                                         CELL_SIZE, CELL_SIZE,
                                         QPen(Qt::lightGray), QBrush(color));
            Q_UNUSED(rect);
        }
    }
    // 绘制玩家位置（圆）
    scene_->addEllipse(playerCol_ * CELL_SIZE + 4, playerRow_ * CELL_SIZE + 4,
                       CELL_SIZE - 8, CELL_SIZE - 8,
                       QPen(Qt::black), QBrush(QColor("#3B5BA5")));
}

void MazeGame::keyPressEvent(QKeyEvent* event) {
    if (gameEnded_) return;

    switch (event->key()) {
        case Qt::Key_Up:    case Qt::Key_W: movePlayer(-1, 0); break;
        case Qt::Key_Down:  case Qt::Key_S: movePlayer(1, 0); break;
        case Qt::Key_Left:  case Qt::Key_A: movePlayer(0, -1); break;
        case Qt::Key_Right: case Qt::Key_D: movePlayer(0, 1); break;
        case Qt::Key_H:     showHint(); break;
        default: QWidget::keyPressEvent(event);
    }
}

void MazeGame::movePlayer(int dr, int dc) {
    int nr = playerRow_ + dr;
    int nc = playerCol_ + dc;
    if (nr < 0 || nr >= rows_ || nc < 0 || nc >= cols_) return;
    if (maze_[nr][nc] == CellType::Wall) return;

    playerRow_ = nr;
    playerCol_ = nc;
    stepsRemaining_--;

    if (playerRow_ == endRow_ && playerCol_ == endCol_) {
        onReachEnd();
        return;
    }
    if (stepsRemaining_ <= 0) {
        gameEnded_ = true;
        statusLabel_->setText(tr("步数耗尽，失败……"));
        emit finished(20, false);
        return;
    }

    redraw();
    statusLabel_->setText(tr("剩余步数 %1 | 按 H 显示 AI 提示（%2/%3）")
                          .arg(stepsRemaining_).arg(maxHints_ - hintsUsed_).arg(maxHints_));
}

void MazeGame::showHint() {
    if (hintsUsed_ >= maxHints_) return;
    hintsUsed_++;
    auto path = bfsShortestPath();
    // TODO（成员 C）：在 scene_ 上以渐变色绘制路径
    qDebug() << "[Maze] showHint, path length =" << path.size();
}

void MazeGame::onReachEnd() {
    gameEnded_ = true;
    int score = 60 + (maxHints_ - hintsUsed_) * 10;  // 少用提示分数更高
    statusLabel_->setText(tr("到达终点！分数 %1").arg(score));
    emit finished(score, true);
}

} // namespace SA
