# src/games/ — 小游戏目录

本目录由**成员 C**维护，存放 6 个小游戏的实现。每个小游戏均派生自 `src/common/MiniGame.h`。

## 计划实现

| 文件 | 类名 | 学科 | 预计完成日期 |
|---|---|---|---|
| `MinesweeperGame.h/.cpp` | MinesweeperGame | 程设 | 5/19 |
| `BlackjackGame.h/.cpp`   | BlackjackGame   | 程设 | 5/20 |
| `TicTacToeGame.h/.cpp`   | TicTacToeGame   | AI引论 | 5/22 |
| `MatrixGame.h/.cpp`      | MatrixGame      | 线代 | 5/23 |
| `MazeGame.h/.cpp`        | MazeGame        | AI引论 | 5/25 |
| `CalculusGame.h/.cpp`    | CalculusGame    | 高数 | 5/26 |

每个小游戏的玩法设计详见 `docs/小游戏玩法设计.docx`。

## 开发模板

```cpp
// MinesweeperGame.h
#include "common/MiniGame.h"

namespace SA {
class MinesweeperGame : public MiniGame {
    Q_OBJECT
public:
    explicit MinesweeperGame(QWidget* parent = nullptr);

    void start() override;
    void reset() override;
    MiniGameType type() const override { return MiniGameType::Minesweeper; }
    QString displayName() const override { return tr("扫雷"); }

private:
    // 内部数据成员……
};
} // namespace SA
```

完成一个小游戏后：
1. 在 `CMakeLists.txt` 的 `PROJECT_SOURCES` 中添加新文件
2. 在 `GameManager::onMiniGameFinished()` 中添加对应的奖惩规则
