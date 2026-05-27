# 成员 A 工作进度

> 这份文档记录我（成员 A）的工作完成情况，方便和队友同步。

## ✅ 已完成（5/14 由 Claude 帮做）

### 代码部分

| 文件 | 完成内容 |
|---|---|
| `src/story/StoryEngine.cpp` | 4 个 TODO 全部补完：节点跳转、选项处理、effects 应用、条件表达式求值 |
| `src/story/StoryEngine.h` | 增加 readVariable、evaluateAtomic、effectsApplied 信号 |
| `src/story/Subjects.cpp` | 4 个学科派生类的 interact() 实现 |
| `src/common/Subject.h` | 增加 interactionRequested 信号 |
| `src/ui/MainWindow.cpp` | 接入 StoryEngine，对话页能真正跑剧本 |
| `src/ui/MainWindow.h` | 增加 storyEngine_/dialogWindow_ 成员 |
| `assets/resources.qrc` | 注册所有 14 个 JSON 剧本资源 |

### 剧本部分

| 文件 | 节点数 | 状态 |
|---|---|---|
| `week1.json` | 约 30 个节点 | ✅ 完整版（覆盖穿越开场 + 第一天上课/翘课分支） |
| `week2.json` | 8 个骨架节点 | 🚧 需要扩写台词 |
| `week3.json` | 8 个骨架节点 | 🚧 需要扩写期中考剧情 |
| `week4.json` | 9 个骨架节点 | 🚧 需要扩写信物+出游剧情 |
| `week5.json` | 6 个骨架节点 | 🚧 需要扩写期末告别 |
| `route_progdesign.json` | 已有（之前的）| ✅ 可用 |
| `route_calculus.json` | 12 个节点 | ✅ 完整版（含哲学问题） |
| `route_linearalgebra.json` | 已有（之前的）| ✅ 可用（含黑化倾向） |
| `route_ai.json` | 14 个节点 | ✅ 完整版（含 AI 哲学讨论） |
| `ending_1_good_grade.json` | 7 个节点 | ✅ 完整版（默认结局） |
| `ending_2_best_love.json` | 已有 | ✅ 可用 |
| `ending_3_farewell.json` | 8 个节点 | ✅ 完整版（告别结局） |
| `ending_4_eternal_la.json` | 5 个节点 | ✅ 已修空 choices bug |
| `ending_5_hospital.json` | 10 个节点 | ✅ 已修空 choices bug |

## ✅ 本轮新增完成（5/23 by Claude 协助）

### 剧本扩写
- **week2.json** —— 8 → **34 节点**，结构：早起选择 → 程设小测 → 午饭线代/AI 二选 → 高数下午课 → 晚自习图书馆，含线代 `darkness>=25` 黑化变体
- **week3.json** —— 8 → **30 节点**，结构：期中倒计时 → 4 学科主攻支线 × 3 节点 → 考试日（先易/硬刚）→ 考后按 affinity ≥ 25 condition 分流到不同陪同结尾
- **week4.json** —— 9 → **36 节点**，结构：信物 4 路（`affinity.X >= 50`）+ 无信物保底 → 出游 2 目的地 × 5 陪同分支（4 角色 + 独自）
- **week5.json** —— 6 → **30 节点**，结构：4 角色最后对话 × 3 节点 → 期末考 → 光门 → 最终选择 4 路（含 `darkness >= 60` 黑化锁定结局）

### 已修 bug
- `la_endgame` 节点空 choices 导致对话死锁
- 5 个 ending 文件的终点节点同样空 choices（统一补「（回到主菜单）」选项触发 `scriptFinished`）
- `end2_leave_anyway` 的 TODO 注释文本改为正式旁白

### 已重构
- 4 个 route_*.json 全部加 hub 分流节点，把过去无引用的 `lv2`/`endgame` 节点接通；纯剧本侧解决，不动 C++

### 已建工具
- [tests/simulate_routes.py](../tests/simulate_routes.py) —— 5 条主路径自动跑通验证
- [tests/simulate_boosted.py](../tests/simulate_boosted.py) —— hub 分流 + 信物阈值 + 黑化结局 25 个用例自动验证
- 全部 30 个测试用例 PASS

### 已写文档
- [作业报告_成员A_第1-3章.md](作业报告_成员A_第1-3章.md) —— 功能介绍 + 剧情系统设计 + 5 结局介绍
- [录屏脚本.md](录屏脚本.md) —— 4 分钟分秒级演示流程

### 已验证编译
- `cmake --build build` 25/25 编译单元通过，`StudyAdventure.app` 生成成功
- Qt 6.11.1 / qtbase + qtmultimedia + qtsvg + qtdeclarative 已确认本机可用

## 🚧 还剩待办

### 优先级 P0

1. **联调测试**——和成员 B/C 一起跑实际 GUI，确认 dialog 渲染、effects 触发动画、地图 → 对话场景切换都正常（依赖队友进度）
2. **跨剧本跳转机制**——`end2_leave_anyway` 设计上应导向 End3，需要和成员 C 对齐 GameManager 是否在该节点 `scriptFinished` 后改判结局

### 优先级 P2（可选改进）
3. 给 `StoryEngine::loadScript` 加可选 `startNode` 参数，让 route 池可由 GameManager 显式分发（比 hub 方案更优雅，但需改 C++ 与成员 C 协调）

## 📝 剧本写作技巧（自己用）

- 每个节点台词 1-3 句话，**不要写过长段落**（对话框装不下）
- 每周剧本 30-50 节点为佳
- 选项 2-3 个最好
- effects 数值：好感度 ±2~5、压力 ±3~10、黑化 ±2~8
- 黑化倾向的剧情用 `condition` 控制（如 `"darkness >= 30"`）

## 🔧 我做的代码模块使用方式

### 加载剧本
```cpp
storyEngine_->loadScript(":/scripts/week1.json");
```

### 剧本结束信号
```cpp
connect(storyEngine_, &StoryEngine::scriptFinished, this, []() {
    // 自动回到地图场景
});
```

### 测试条件表达式
支持的语法：
```
"affinity.ProgDesign >= 50"
"stress < 80 && darkness < 50"
"week >= 3 || affinity.AIIntro > 30"
```

变量：`affinity.程设/高数/线代/AI`、`stress`、`darkness`、`week`、`day`
运算符：`> >= < <= == !=`、`&&`、`||`
