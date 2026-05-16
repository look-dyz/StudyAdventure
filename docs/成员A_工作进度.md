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
| `ending_4_eternal_la.json` | 已有 | ✅ 可用（黑化结局） |
| `ending_5_hospital.json` | 11 个节点 | ✅ 完整版（崩溃结局） |

## 🚧 还需要我做的

### 优先级 P0（5/15-5/27 完成）

1. **扩写 week2.json** - 8 个骨架节点需要扩写台词，并在合适位置增加分支。目标 30+ 节点。
2. **扩写 week3.json** - 期中考剧情。可以让玩家选择"和谁一起复习"。目标 25+ 节点。
3. **扩写 week4.json** - 信物剧情和出游剧情。4 个角色的 4 种信物。目标 35+ 节点。
4. **扩写 week5.json** - 期末告别。每个角色的最后告白对话。目标 30+ 节点。
5. **测试所有剧情线** - 走完 5 条路径（4 学科线 + 默认），确保每个结局都能正确触发。

### 优先级 P1

6. **撰写作业报告 1-3 章** - 功能介绍 + 剧情系统设计 + 5 结局介绍（约 3-4 页）
7. **写录屏脚本** - 4 分钟分秒级安排

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
