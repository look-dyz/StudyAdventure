# 学海漫游：异世界的信科少女

> Study Adventure: The Informatics Girl in Another World

北京大学信息科学技术学院程序设计课程大作业项目。一款乙女向养成模拟游戏，玩家扮演北大信科大一女生，期末通宵晕倒后穿越至异世界，与拟人化的四个学科角色（程设、高数、线代、人工智能引论）共度一学期，通过剧情推进、自由探索、知识小游戏培养角色好感度，最终触发多种结局。

## ✨ 项目特色

- 🎬 **完整剧情系统**：5 周校园生活剧情、4 条学科好感度路线、5 种不同结局
- 🎮 **6 个知识小游戏**：扫雷、21点、井字棋、矩阵运算、AI 迷宫逃生、高数答题
- 🧠 **算法可视化**：Minimax + α-β 剪枝（井字棋 AI）、BFS 最短路径（迷宫提示）
- 💖 **多维数值系统**：每个学科独立好感度 + 玩家压力值 + 线代黑化值
- 🎨 **Qt 6 跨平台界面**：Windows / macOS / Linux 均可运行

## 🛠 技术栈

- **语言**：C++ 17
- **GUI 框架**：Qt 6（Widgets + Graphics View）
- **构建系统**：CMake 3.16+
- **数据格式**：JSON（剧本、存档、题库均外置）

## 📂 项目结构

```
StudyAdventure/
├── CMakeLists.txt         # 顶层构建配置
├── README.md              # 本文件
├── LICENSE                # MIT 开源许可
├── .gitignore             # Git 忽略规则
│
├── src/                   # 源代码
│   ├── main.cpp           # 程序入口
│   ├── common/            # 公共基类与常量（三人共享，修改需群里通告）
│   │   ├── MiniGame.h     # 小游戏抽象基类
│   │   ├── Subject.h      # 学科角色抽象基类
│   │   └── Constants.h    # 全局常量
│   ├── core/              # 数值与存档系统（成员C负责）
│   │   ├── Player.h/.cpp
│   │   ├── GameManager.h/.cpp
│   │   └── SaveManager.h/.cpp
│   ├── story/             # 剧情系统（成员A负责）
│   │   ├── StoryEngine.h/.cpp
│   │   ├── DialogWindow.h/.cpp
│   │   └── EndingJudge.h/.cpp
│   ├── ui/                # 用户界面（成员B负责）
│   │   ├── MainWindow.h/.cpp
│   │   ├── MainMenu.h/.cpp
│   │   └── StatusBar.h/.cpp
│   ├── map/               # 地图场景（成员B负责）
│   │   └── MapScene.h/.cpp
│   └── games/             # 6 个小游戏（成员C负责）
│       ├── MinesweeperGame.h/.cpp
│       ├── BlackjackGame.h/.cpp
│       ├── TicTacToeGame.h/.cpp
│       ├── MatrixGame.h/.cpp
│       ├── MazeGame.h/.cpp
│       └── CalculusGame.h/.cpp
│
├── assets/                # 资源文件
│   ├── images/            # 图像资源
│   │   ├── characters/    # 角色立绘
│   │   ├── backgrounds/   # 背景图
│   │   └── ui/            # UI 素材
│   ├── audio/             # 音频资源
│   │   ├── bgm/           # 背景音乐
│   │   └── sfx/           # 音效
│   ├── scripts/           # 剧本 JSON 文件
│   └── data/              # 题库等数据文件
│
└── docs/                  # 项目文档
    ├── 功能设计文档.docx
    ├── 项目开发规划.docx
    ├── 工作任务详解与时间线.docx
    ├── 小游戏玩法设计.docx
    └── 接口约定.md
```

## 🚀 编译和运行

### 环境要求

- **Qt 6.2 或更高版本**（[官方下载](https://www.qt.io/download-qt-installer)，免费开源版即可）
- **CMake 3.16 或更高版本**
- **C++17 兼容编译器**：MSVC 2019+、GCC 9+、Clang 10+

### Windows（使用 Qt Creator，推荐）

1. 安装 Qt 6 时勾选 MinGW 或 MSVC 编译器
2. 用 Qt Creator 打开本目录下的 `CMakeLists.txt`
3. 配置项目（选择 Qt 6 Kit）→ 点击左下角绿色三角形运行

### Windows / macOS / Linux（使用命令行）

```bash
# 进入项目目录
cd StudyAdventure

# 创建构建目录
mkdir build && cd build

# 配置（指定 Qt 6 安装路径，按实际修改）
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64

# 编译
cmake --build . --parallel

# 运行
./StudyAdventure        # Linux/macOS
StudyAdventure.exe      # Windows
```

## 👥 团队分工

| 成员 | 负责模块 | 主要类 |
|:---:|:---|:---|
| **成员 A** | 剧情核心 | StoryEngine、DialogWindow、EndingJudge、JSON 剧本 |
| **成员 B** | 界面核心 | MainWindow、MapScene、StatusBar、美术资源整合 |
| **成员 C** | 玩法核心 | 6 个 MiniGame、Player、GameManager、SaveManager |

详见 `docs/工作任务详解与时间线.docx`。

## 📅 开发周期

- **开发期**：2026 年 5 月 13 日 — 6 月 1 日（共 20 天）
- **录屏期**：6 月 2 日 — 6 月 5 日
- **初版提交 DDL**：**6 月 6 日 23:59**
- **最终提交 DDL**：7 月 6 日

## 🔀 分支策略

- `main`：稳定版本，仅在阶段性里程碑合入
- `develop`：默认开发分支，所有 feature 合入这里
- `feature/story-*`：成员 A 的功能分支
- `feature/ui-*`：成员 B 的功能分支
- `feature/game-*`：成员 C 的功能分支

提交信息格式：`[模块] 简述`，例如 `[story] 实现分支跳转逻辑`、`[ui] 修复地图点击响应`。

## 📜 开源许可

本项目采用 MIT License 开源，详见 [LICENSE](./LICENSE) 文件。

---

*愿我们在异世界，与高数同窗、与线代共眠、与程设并肩、与 AI 同行。* 🌸
