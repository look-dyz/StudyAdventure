# 变更日志

> 记录每个版本的功能变更。格式参考 [Keep a Changelog](https://keepachangelog.com/zh-CN/1.1.0/)。

## [Unreleased]

### 待办
- 成员 A：完成 StoryEngine 的分支跳转、条件判断、effects 应用
- 成员 B：完成 4 张地图场景的背景图整合
- 成员 C：补全 6 个小游戏的算法细节（详见各文件的 TODO 标记）

---

## [0.1.0] - 2026-05-13

### 新增 ✨
- 项目骨架搭建完成，包含 5 大模块（剧情/界面/地图/玩法/数值）
- 实现 `MiniGame` 和 `Subject` 两个核心抽象基类
- 实现 `Player` 数值系统（4 学科好感 + 压力 + 黑化 + 日期）
- 实现 `GameManager` 单例（场景切换、小游戏奖惩调度）
- 实现 `EndingJudge` 结局判定（5 个结局的优先级链）
- 实现 `MainWindow` 主框架，含 5 个场景页面
- 实现 `MainMenu` 主菜单组件
- 实现 `StatusBar` 顶部状态栏（信号槽驱动自动刷新）
- 实现 `MapScene` + `Location` 真实可点击地图
- 实现 `DialogWindow` 对话框组件
- 6 个小游戏代码框架：
  - `MinesweeperGame`（扫雷）
  - `BlackjackGame`（21 点，**完整可玩**）
  - `TicTacToeGame`（井字棋，**完整可玩**，含 Minimax 框架）
  - `MatrixGame`（矩阵运算，含 `Matrix` 类与运算符重载）
  - `MazeGame`（AI 迷宫，含 BFS 框架）
  - `CalculusGame`（高数答题，**完整可玩**，已接 JSON 题库）
- 4 个学科派生类（ProgDesign/Calculus/LinearAlgebra/AIIntro）
- 示例剧本 JSON：`week1.json`、`route_progdesign.json`、`route_linearalgebra.json`
- 示例结局剧本：`ending_2_best_love.json`、`ending_4_eternal_la.json`
- 示例题库：`calculus_questions.json`（5 道高数题）
- 4 个角色占位 SVG 立绘（程设/高数/线代/AI 引论）
- 应用图标 SVG

### 工程化 🛠
- CMakeLists.txt 跨平台构建配置（C++17 + Qt 6 Widgets + Multimedia）
- GitHub Actions CI（自动在 Linux/Windows/macOS 上编译验证）
- PR / Issue 模板
- `CODEOWNERS` 自动 review 分配
- `.editorconfig` 跨编辑器统一格式
- `.clang-format` C++ 代码风格
- `.gitignore` 完整忽略规则
- 已在 Qt5 沙箱中通过完整编译验证（生产环境用 Qt6）

### 文档 📚
- `README.md` 项目主页
- `CONTRIBUTING.md` 协作贡献指南
- `docs/接口约定.md` 三人协作核心约定
- `docs/开发指南.md` 新手入门
- `docs/JSON剧本格式.md` 剧本速查
- `docs/功能设计文档`（docx + md）
- `docs/项目开发规划`（docx + md，含甘特图、风险）
- `docs/小游戏玩法设计`（docx + md，6 个游戏详解）
- `docs/工作任务详解与时间线`（docx + md，每人每天任务）
- `docs/README.md` 文档导航

---

## 版本号约定

遵循 [语义化版本](https://semver.org/lang/zh-CN/)：

- `0.x.y` — 开发期（5/13 - 6/06）
- `1.0.0` — 6/6 初版提交版本
- `1.x.y` — 6/6 后的优化迭代
- `2.0.0` — 7/6 最终提交版本
