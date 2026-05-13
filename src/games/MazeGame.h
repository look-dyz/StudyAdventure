/**
 * @file MazeGame.h
 * @brief AI 迷宫逃生（对应 AI 引论学科）
 *
 * 玩法：玩家在 15x15 迷宫中找出口，可请求 AI 用 BFS 提示最短路径。
 * 算法重点：递归回溯生成迷宫 + BFS 最短路径。
 *
 * 负责人：成员 C   预计完成：5/25
 */
#ifndef STUDYADVENTURE_MAZEGAME_H
#define STUDYADVENTURE_MAZEGAME_H

#include "common/MiniGame.h"
#include <vector>
#include <utility>

class QGraphicsView;
class QGraphicsScene;
class QLabel;

namespace SA {

class MazeGame : public MiniGame {
    Q_OBJECT
public:
    explicit MazeGame(QWidget* parent = nullptr);
    ~MazeGame() override = default;

    void start() override;
    void reset() override;
    MiniGameType type() const override { return MiniGameType::Maze; }
    QString displayName() const override { return tr("AI 迷宫逃生"); }

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    enum class CellType { Wall, Empty, Trap, Knowledge, Start, End };

    int rows_ = 15;
    int cols_ = 15;
    int playerRow_ = 0;
    int playerCol_ = 0;
    int endRow_ = 0;
    int endCol_ = 0;
    int stepsRemaining_ = 60;
    int hintsUsed_ = 0;
    int maxHints_ = 3;
    bool gameEnded_ = false;

    std::vector<std::vector<CellType>> maze_;

    QGraphicsView* view_ = nullptr;
    QGraphicsScene* scene_ = nullptr;
    QLabel* statusLabel_ = nullptr;

    void generateMaze();                                  ///< 递归回溯生成
    void carve(int r, int c);                              ///< DFS 挖通路
    void placeSpecialCells();                              ///< 放置陷阱/知识格
    std::vector<std::pair<int,int>> bfsShortestPath();     ///< BFS 最短路径

    void redraw();
    void movePlayer(int dr, int dc);
    void showHint();
    void onReachEnd();
};

} // namespace SA

#endif // STUDYADVENTURE_MAZEGAME_H
