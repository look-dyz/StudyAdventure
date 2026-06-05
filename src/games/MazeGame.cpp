#include "MazeGame.h"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>

#include <QLabel>
#include <QPushButton>

#include <QVBoxLayout>
#include <QStackedLayout>

#include <QKeyEvent>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>

#include <queue>
#include <random>
#include <algorithm>

namespace SA {

constexpr int CELL_SIZE = 28;

MazeGame::MazeGame(QWidget* parent)
    : MiniGame(parent)
{
    // 整体布局（两页）
    stackedLayout_ = new QStackedLayout(this);

            // =====================================================
            // 玩法说明页
            // =====================================================

    introPage_ = new QWidget(this);

    {
        auto* layout =
            new QVBoxLayout(introPage_);

        auto* title =
            new QLabel(tr("AI迷宫玩法说明"));

        title->setAlignment(Qt::AlignCenter);

        title->setStyleSheet(
            "font-size:24px;"
            "font-weight:bold;"
            "color:#8B1A1A;"
            );

        auto* introText =
            new QLabel(
                tr(
                    "【游戏目标】\n"
                    "在步数耗尽前到达终点。\n\n"

                    "【操作方式】\n"
                    "WASD / 方向键 控制移动。\n\n"

                    "【特殊格子】\n"
                    "红色：陷阱格（额外扣步数）\n"
                    "黄色：知识格（奖励步数）\n\n"

                    "【AI提示】\n"
                    "按 H 显示最短路径。\n"
                    "提示次数有限。\n\n"

                    "【胜利条件】\n"
                    "成功到达终点。"
                    )
                );

        introText->setWordWrap(true);

        introText->setStyleSheet(
            "font-size:16px;"
            "padding:20px;"
            );

        auto* easyBtn =
            new QPushButton(tr("简单"));

        auto* normalBtn =
            new QPushButton(tr("普通"));

        auto* hardBtn =
            new QPushButton(tr("困难"));

        easyBtn->setMinimumHeight(45);
        normalBtn->setMinimumHeight(45);
        hardBtn->setMinimumHeight(45);

        easyBtn->setStyleSheet(
            "font-size:18px;"
            "background:#B8F2C8;"
            "border-radius:10px;"
            );

        normalBtn->setStyleSheet(
            "font-size:18px;"
            "background:#F5E6CA;"
            "border-radius:10px;"
            );

        hardBtn->setStyleSheet(
            "font-size:18px;"
            "background:#FFB3B3;"
            "border-radius:10px;"
            );

        connect(easyBtn,
                &QPushButton::clicked,
                this,
                [this]() {

                    difficulty_ = Difficulty::Easy;

                    start();

                    reset();

                    stackedLayout_->setCurrentWidget(gamePage_);

                    setFocus();
                });

        connect(normalBtn,
                &QPushButton::clicked,
                this,
                [this]() {

                    difficulty_ = Difficulty::Normal;

                    start();

                    reset();

                    stackedLayout_->setCurrentWidget(gamePage_);

                    setFocus();
                });

        connect(hardBtn,
                &QPushButton::clicked,
                this,
                [this]() {

                    difficulty_ = Difficulty::Hard;

                    start();

                    reset();

                    stackedLayout_->setCurrentWidget(gamePage_);

                    setFocus();
                });

        easyBtn->setMinimumHeight(45);
        normalBtn->setMinimumHeight(45);
        hardBtn->setMinimumHeight(45);

        easyBtn->setStyleSheet(
            "font-size:18px;"
            "background:#B8F2C8;"
            "border-radius:10px;"
            );

        normalBtn->setStyleSheet(
            "font-size:18px;"
            "background:#F5E6CA;"
            "border-radius:10px;"
            );

        hardBtn->setStyleSheet(
            "font-size:18px;"
            "background:#FFB3B3;"
            "border-radius:10px;"
            );

        layout->addStretch();

        layout->addWidget(title);

        layout->addWidget(introText);

        layout->addWidget(easyBtn);

        layout->addWidget(normalBtn);

        layout->addWidget(hardBtn);

        layout->addStretch();
    }

            // =====================================================
            // 正式游戏页
            // =====================================================

    gamePage_ = new QWidget(this);

    {
        auto* layout =
            new QVBoxLayout(gamePage_);

        auto* title =
            new QLabel(displayName());

        title->setAlignment(Qt::AlignCenter);

        title->setStyleSheet(
            "font-size:20px;"
            "font-weight:bold;"
            "color:#8B1A1A;"
            );

        statusLabel_ =
            new QLabel;

        scene_ =
            new QGraphicsScene(this);

        view_ =
            new QGraphicsView(scene_);

        view_->setMinimumSize(700, 700);

        layout->addWidget(title);

        layout->addWidget(statusLabel_);

        layout->addWidget(view_);

        layout->addStretch();
    }

            // =====================================================
            // 页面加入
            // =====================================================

    stackedLayout_->addWidget(introPage_);

    stackedLayout_->addWidget(gamePage_);

    stackedLayout_->setCurrentWidget(introPage_);

            // 关键：MazeGame 自己接收键盘
    setFocusPolicy(Qt::StrongFocus);

    timer_ = new QTimer(this);

    connect(timer_,
            &QTimer::timeout,
            this,
            [this]() {

                if(gameEnded_)
                    return;

                timeRemaining_--;

                statusLabel_->setText(
                    tr("剩余步数 %1 | 剩余时间 %2 秒 | H键提示（%3/%4）")
                        .arg(stepsRemaining_)
                        .arg(timeRemaining_)
                        .arg(maxHints_ - hintsUsed_)
                        .arg(maxHints_)
                    );

                if(timeRemaining_ <= 0) {

                    gameEnded_ = true;

                    timer_->stop();

                    QMessageBox::warning(
                        this,
                        tr("时间结束"),
                        tr("时间耗尽，挑战失败！")
                        );

                    emit finished(10, false);
                }
            });
}

void MazeGame::start()
{
    switch (difficulty_) {

        case Difficulty::Easy:
            stepsRemaining_ = 60;
            timeRemaining_ = 60;
            break;

        case Difficulty::Normal:
            stepsRemaining_ = 45;
            timeRemaining_ = 30;
            break;

        case Difficulty::Hard:
            stepsRemaining_ = 30;
            timeRemaining_ = 15;
            break;
    }

    stackedLayout_->setCurrentWidget(introPage_);
}

void MazeGame::reset()
{
    // 必须奇数尺寸
    if(rows_ % 2 == 0) rows_--;
    if(cols_ % 2 == 0) cols_--;

    maze_.assign(
        rows_,
        std::vector<CellType>(
            cols_,
            CellType::Wall
            )
        );

    hintsUsed_ = 0;

    gameEnded_ = false;

    do {

        maze_.assign(
            rows_,
            std::vector<CellType>(
                cols_,
                CellType::Wall
                )
            );

        generateMaze();

        placeSpecialCells();

        playerRow_ = 0;
        playerCol_ = 0;

        endRow_ = rows_ - 2;
        endCol_ = cols_ - 2;

        maze_[playerRow_][playerCol_]
            = CellType::Start;

        maze_[endRow_][endCol_]
            = CellType::End;

    } while(!isMazeSolvableWithinSteps());



    redraw();

    statusLabel_->setText(
        tr("剩余步数 %1 | H键提示（%2/%3）")
            .arg(stepsRemaining_)
            .arg(maxHints_ - hintsUsed_)
            .arg(maxHints_)
        );

    timer_->start(1000);

            // 核心：重新拿焦点
    setFocus();
}

void MazeGame::generateMaze()
{
    for(auto& row : maze_) {

        for(auto& cell : row) {

            cell = CellType::Wall;
        }
    }

    carve(0, 0);
}

void MazeGame::carve(int r, int c)
{
    maze_[r][c] = CellType::Empty;

    std::vector<std::pair<int,int>> dirs = {
        {-1,0},
        {1,0},
        {0,-1},
        {0,1}
    };

    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::shuffle(dirs.begin(), dirs.end(), gen);

    for(const auto& d : dirs) {

        int dr = d.first;
        int dc = d.second;

        int nr = r + dr * 2;
        int nc = c + dc * 2;

                // 越界
        if(nr < 0 || nr >= rows_
            || nc < 0 || nc >= cols_) {
            continue;
        }

                // 已访问
        if(maze_[nr][nc] != CellType::Wall) {
            continue;
        }

                // 挖通中间墙
        maze_[r + dr][c + dc]
            = CellType::Empty;

        carve(nr, nc);
    }
}

void MazeGame::placeSpecialCells()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::uniform_int_distribution<> rdist(
        0,
        rows_ - 1
        );

    std::uniform_int_distribution<> cdist(
        0,
        cols_ - 1
        );

            // 陷阱
    for(int i = 0; i < 8; ++i) {

        int r = rdist(gen);
        int c = cdist(gen);

        if((r == 0 && c == 0)
            || (r == rows_-1 && c == cols_-1))
            continue;

        if(maze_[r][c] == CellType::Empty)
            maze_[r][c] = CellType::Trap;
    }

            // 知识格
    for(int i = 0; i < 5; ++i) {

        int r = rdist(gen);
        int c = cdist(gen);

        if((r == 0 && c == 0)
            || (r == rows_-1 && c == cols_-1))
            continue;

        if(maze_[r][c] == CellType::Empty)
            maze_[r][c] = CellType::Knowledge;
    }
}

std::vector<std::pair<int,int>>
MazeGame::bfsShortestPath()
{
    std::queue<std::pair<int,int>> q;

    std::vector<std::vector<bool>>
        vis(rows_,
            std::vector<bool>(
                cols_,
                false
                ));

    std::vector<std::vector<
        std::pair<int,int>>>
        prev(rows_,
             std::vector<
                 std::pair<int,int>>(
                 cols_,
                 {-1,-1}
                 ));

    q.push({playerRow_, playerCol_});

    vis[playerRow_][playerCol_]
        = true;

    std::vector<std::pair<int,int>>
        dirs = {
            {-1,0},
            {1,0},
            {0,-1},
            {0,1}
        };

    while(!q.empty()) {

        auto cur = q.front();
        q.pop();

        int r = cur.first;
        int c = cur.second;

        if(r == endRow_
            && c == endCol_) {

            break;
        }

        for(const auto& d : dirs) {

            int nr = r + d.first;
            int nc = c + d.second;

            if(nr < 0 || nr >= rows_
                || nc < 0 || nc >= cols_)
                continue;

            if(vis[nr][nc])
                continue;

            if(maze_[nr][nc]
                == CellType::Wall)
                continue;

            vis[nr][nc] = true;

            prev[nr][nc] = {r,c};

            q.push({nr,nc});
        }
    }

    std::vector<std::pair<int,int>>
        path;

    int r = endRow_;
    int c = endCol_;

    while(!(r == playerRow_
             && c == playerCol_)) {

        path.push_back({r,c});

        auto p = prev[r][c];

        if(p.first == -1)
            break;

        r = p.first;
        c = p.second;
    }

    std::reverse(path.begin(),
                 path.end());

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

    view_->centerOn(
        playerCol_ * CELL_SIZE,
        playerRow_ * CELL_SIZE
        );
}

void MazeGame::keyPressEvent(QKeyEvent* event)
{
    if(gameEnded_) return;

    switch(event->key()) {

        case Qt::Key_W:
        case Qt::Key_Up:
            movePlayer(-1, 0);
            break;

        case Qt::Key_S:
        case Qt::Key_Down:
            movePlayer(1, 0);
            break;

        case Qt::Key_A:
        case Qt::Key_Left:
            movePlayer(0, -1);
            break;

        case Qt::Key_D:
        case Qt::Key_Right:
            movePlayer(0, 1);
            break;

        case Qt::Key_H:
            showHint();
            break;

        default:
            QWidget::keyPressEvent(event);
    }
}

void MazeGame::movePlayer(int dr, int dc) {
    int nr = playerRow_ + dr;
    int nc = playerCol_ + dc;
    if (nr < 0 || nr >= rows_ || nc < 0 || nc >= cols_) return;
    if (maze_[nr][nc] == CellType::Wall) return;

    playerRow_ = nr;
    playerCol_ = nc;
    //踩到陷阱
    if(maze_[nr][nc]
        == CellType::Trap) {

        stepsRemaining_ -= 3;

        statusLabel_->setText(
            tr("踩到陷阱！额外扣 3 步")
            );
    }
    //踩到知识格
    if(maze_[nr][nc]
        == CellType::Knowledge) {

        stepsRemaining_ += 5;

        statusLabel_->setText(
            tr("获得知识补给！+5 步")
            );

        maze_[nr][nc]
            = CellType::Empty;
    }

    stepsRemaining_--;

    if (playerRow_ == endRow_ && playerCol_ == endCol_) {
        onReachEnd();
        return;
    }
    if (stepsRemaining_ <= 0) {
        gameEnded_ = true;
        timer_->stop();
        statusLabel_->setText(tr("步数耗尽，失败……"));
        QMessageBox::warning(
            this,
            tr("游戏失败"),
            tr("步数耗尽，挑战失败！")
            );
        emit finished(20, false);
        return;
    }

    redraw();
    setFocus();
    statusLabel_->setText(tr("剩余步数 %1 | 按 H 显示 AI 提示（%2/%3）")
                          .arg(stepsRemaining_).arg(maxHints_ - hintsUsed_).arg(maxHints_));
}

void MazeGame::showHint() {
    if (hintsUsed_ >= maxHints_) return;
    hintsUsed_++;
    auto path = bfsShortestPath();
    // TODO（成员 C）：在 scene_ 上以渐变色绘制路径

    for(size_t i = 0;
         i < path.size();
         ++i) {

        int r = path[i].first;
        int c = path[i].second;

        QColor color(
            100,
            180,
            255,
            80
            );

        scene_->addRect(
            c * CELL_SIZE + 8,
            r * CELL_SIZE + 8,
            CELL_SIZE - 16,
            CELL_SIZE - 16,
            QPen(Qt::NoPen),
            QBrush(color)
            );
    }

    qDebug() << "[Maze] showHint, path length =" << path.size();
}

bool MazeGame::isMazeSolvableWithinSteps()
{
    auto path = bfsShortestPath();

    if(path.empty())
        return false;

    int cost = path.size();

            // 计算路径中的陷阱额外代价
    for(const auto& p : path) {

        int r = p.first;
        int c = p.second;

        if(maze_[r][c] == CellType::Trap) {

            cost += 3;
        }
    }

    qDebug()
        << "[Maze] shortest cost ="
        << cost
        << "steps ="
        << stepsRemaining_;

    return cost <= stepsRemaining_;
}

void MazeGame::onReachEnd() {

    gameEnded_ = true;

    timer_->stop();

    int score =
        60 + (maxHints_ - hintsUsed_) * 10;

    statusLabel_->setText(
        tr("到达终点！分数 %1").arg(score)
        );

    QMessageBox::information(
        this,
        tr("游戏胜利"),
        tr("成功走出迷宫！\n最终得分：%1")
            .arg(score)
        );

    emit finished(score, true);
}

} // namespace SA
