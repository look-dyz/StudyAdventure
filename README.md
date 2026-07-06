<div align="center">

# 🌸 学海漫游：异世界的信科少女

**Study Adventure: The Informatics Girl in Another World**

*一款 C++/Qt 写的乙女向养成模拟游戏 · 北京大学信息科学技术学院程设课程大作业*

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Qt 6](https://img.shields.io/badge/Qt-6.2%2B-41CD52.svg)
![CMake](https://img.shields.io/badge/CMake-3.16%2B-064F8C.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20macOS%20%7C%20Linux-lightgrey.svg)
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)

</div>

---

## 📦 大作业提交清单

| 材料 | 位置 |
|---|---|
| **① 作业报告** | [`docs/实验报告.pdf`](docs/实验报告.pdf) （Markdown 源文件：[`docs/实验报告.md`](docs/实验报告.md)） |
| **② 源代码** | 本仓库全部内容（[编译方法见下方](#-快速开始)） |
| **③ 演示录屏** | [`docs/演示视频.mp4`](docs/演示视频.mp4) （4 分钟分秒级脚本见 [`docs/录屏脚本.md`](docs/录屏脚本.md)） |

**开发团队**：代易瓒（剧情核心）· 闫瑾（界面核心）· 沈昱萱（玩法核心）

---

## 📖 故事简介

北大信科大一女生，期末复习的深夜，桌前一阵眩晕——再睁眼，已身处异世界。

四个拟人化的学科角色站在你面前：理性冷淡的**程设**、神秘抽象的**高数**、阴郁占有的**线代**、酷感未来的**AI 引论**。一学期的时间，你的选择将决定与谁产生羁绊，又会迎来怎样的结局。

> 「学海漫游，是一场关于学习与成长的恋爱模拟。」

## ✨ 核心特色

- 🎬 **完整剧情系统** — 5 周校园生活、4 条学科线、5 种结局
- 🎮 **5 款知识小游戏** — 扫雷 · 21 点 · 井字棋（含 Minimax + α-β 剪枝）· 记忆翻牌 · AI 迷宫（含 BFS 可视化）
- 🧠 **真实算法实现** — Minimax + α-β 剪枝、BFS 最短路径、递归回溯生成迷宫
- 💖 **多维数值系统** — 学科好感度 × 4 + 压力值 + 线代黑化值
- 🎨 **Qt 6 跨平台 GUI** — 一份代码，Windows / macOS / Linux 三平台运行

## 🚀 快速开始

### 环境要求

| 工具 | 最低版本 | 下载 |
|---|---|---|
| Qt | 6.2+ | https://www.qt.io/download-qt-installer |
| CMake | 3.16+ | （随 Qt Creator 安装） |
| C++ 编译器 | C++17 | MSVC 2019+ / GCC 9+ / Clang 10+ |

### 编译 & 运行

#### 方法一：Qt Creator（推荐 · 三分钟启动）

1. 启动 Qt Creator
2. 文件 → 打开文件或项目 → 选择本仓库的 `CMakeLists.txt`
3. 选择 Qt 6 Kit → 点击左下角 ▶ 运行

#### 方法二：命令行

```bash
git clone https://github.com/<your-team>/StudyAdventure.git
cd StudyAdventure
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/<compiler>
cmake --build . --parallel
./StudyAdventure        # Linux/macOS
StudyAdventure.exe       # Windows
```

## 📂 项目结构

```
StudyAdventure/
├── 📄 README.md / CONTRIBUTING.md / CHANGELOG.md / LICENSE
├── 📄 CMakeLists.txt              ← 跨平台构建配置
├── 📄 .clang-format / .editorconfig  ← 代码风格
│
├── 📂 .github/                     ← GitHub 元数据
│   ├── workflows/build.yml         ← CI 自动编译验证（3 平台）
│   ├── ISSUE_TEMPLATE/             ← Bug / 功能建议模板
│   ├── PULL_REQUEST_TEMPLATE.md
│   └── CODEOWNERS                  ← 自动 review 分配
│
├── 📂 src/                         ← 源代码（21 个头文件 + 20 个 cpp）
│   ├── main.cpp
│   ├── common/                     ← 公共基类（三人共享）
│   │   ├── Constants.h
│   │   ├── MiniGame.h / Subject.h.cpp
│   ├── core/                       ← 数值 & 调度（成员 C）
│   │   ├── Player.{h,cpp}
│   │   ├── GameManager.{h,cpp}
│   │   └── SaveManager.{h,cpp}
│   ├── story/                      ← 剧情系统（成员 A）
│   │   ├── StoryEngine.{h,cpp}
│   │   ├── DialogWindow.{h,cpp}
│   │   ├── EndingJudge.{h,cpp}
│   │   └── Subjects.{h,cpp}
│   ├── ui/                         ← 用户界面（成员 B）
│   │   ├── MainWindow.{h,cpp}
│   │   ├── MainMenu.{h,cpp}
│   │   └── StatusBar.{h,cpp}
│   ├── map/                        ← 地图场景（成员 B）
│   │   ├── MapScene.{h,cpp}
│   │   └── Location.{h,cpp}
│   └── games/                      ← 6 个小游戏（成员 C）
│       ├── MinesweeperGame.{h,cpp}
│       ├── BlackjackGame.{h,cpp}
│       ├── TicTacToeGame.{h,cpp}
│       ├── MatrixGame.{h,cpp}
│       ├── MazeGame.{h,cpp}
│       └── CalculusGame.{h,cpp}
│
├── 📂 assets/                      ← 资源文件
│   ├── resources.qrc               ← Qt 资源清单
│   ├── images/                     ← 立绘、背景、UI
│   │   ├── characters/             ← 4 个角色立绘 SVG（占位）
│   │   ├── backgrounds/            ← 4 张地图背景
│   │   └── ui/                     ← 图标 SVG
│   ├── audio/                      ← BGM 和 SFX
│   ├── scripts/                    ← JSON 剧本
│   │   ├── week1.json
│   │   ├── route_progdesign.json
│   │   ├── route_linearalgebra.json
│   │   ├── ending_2_best_love.json
│   │   └── ending_4_eternal_la.json
│   └── data/
│       └── calculus_questions.json
│
└── 📂 docs/                        ← 项目文档（docx + md 双版本）
    ├── README.md                   ← 文档导航
    ├── 接口约定.md                 ← ⭐ 三人协作核心
    ├── 协作流程.md                 ← 日常节奏 + 应急预案
    ├── 开发指南.md                 ← 新手入门 + FAQ
    ├── JSON剧本格式.md             ← 剧本速查
    ├── 功能设计文档.{docx,md}
    ├── 项目开发规划.{docx,md}
    ├── 小游戏玩法设计.{docx,md}
    └── 工作任务详解与时间线.{docx,md}
```

## 👥 团队分工

| 成员 | 模块 | 主要负责 |
|:---:|:---|:---|
| **成员 A** | 剧情核心 | StoryEngine · DialogWindow · EndingJudge · JSON 剧本 |
| **成员 B** | 界面核心 | MainWindow · MapScene · StatusBar · 美术资源 |
| **成员 C** | 玩法核心 | 6 个 MiniGame · Player · GameManager · SaveManager |

详细分工见 [`docs/工作任务详解与时间线.md`](docs/工作任务详解与时间线.md)。

## 📅 开发时间线

```
5/13 ──┐
       │ 阶段一：设计与接口约定
5/15 ──┤
       │ 阶段二：搭骨架（5/17 跑通最小示例 ⭐）
5/18 ──┤
       │ 阶段三：正式开发（10 天并行编码）
5/28 ──┤
       │ 阶段四：联调与测试
5/31 ──┤
       │ 阶段五：写作业报告
6/02 ──┤
       │ 阶段六：录屏 & 最终交付
6/06 ──┴ ★ 初版提交 DDL（23:59）
```

完整规划见 [`docs/项目开发规划.md`](docs/项目开发规划.md)。

## 🛠 技术亮点

### 1. OOP 多态 · 工厂模式
```cpp
Subject* progdesign = new ProgDesignSubject;
MiniGame* game = progdesign->createGame();   // 返回扫雷
game->start();
```

### 2. 单例 · RAII
```cpp
auto& mgr = GameManager::instance();         // 全局唯一实例
mgr.player()->addAffinity(SubjectType::Calculus, +5);
```

### 3. 信号槽解耦 · 模块通信
```cpp
// Player 数值变化 → StatusBar 自动刷新进度条
connect(player, &Player::affinityChanged,
        statusBar, &StatusBar::onAffinityChanged);

// 小游戏结束 → GameManager 统一处理奖惩
connect(game, &MiniGame::finished,
        &GameManager::instance(), &GameManager::onMiniGameFinished);
```

### 4. 数据驱动 · JSON 剧本与代码彻底分离
- 写剧本的人不需要懂 C++
- 改剧情只需要改 JSON，不需要重新编译

### 5. CMake + GitHub Actions
- 一份配置跨 Windows / macOS / Linux 编译
- 每次 push 自动在 3 平台验证编译通过

## 🤝 参与贡献

请阅读 [CONTRIBUTING.md](CONTRIBUTING.md) 了解分支策略、提交规范与 PR 流程。

## 📜 开源许可

本项目采用 [MIT License](LICENSE)，欢迎学习与改进。素材资源（图片/音频）的版权随其原始来源。

## 🌸 致谢

- 感谢北京大学信息科学技术学院的程设课程及助教团队
- 感谢 [Qt 项目](https://www.qt.io/) 提供的优秀跨平台 GUI 框架
- 感谢每一个为这个项目熬过夜的队员 💪

---

<div align="center">

*愿我们在异世界，与高数同窗、与线代共眠、与程设并肩、与 AI 同行。* 🌸

</div>
