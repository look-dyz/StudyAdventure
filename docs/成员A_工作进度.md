# 成员 A 工作进度

> 这份文档记录我（成员 A）的工作完成情况，方便和队友同步。
> **最近更新**：5/23 完成 week2-5 扩写、route 重构、ending bug 修复、LA 调性矫正、剧本测试与文档；并已 merge 成员 C 的 `game_dev` 分支。

---

## ✅ 已完成

### 一、代码模块

| 文件 | 完成内容 |
|---|---|
| `src/story/StoryEngine.cpp` | 4 个 TODO 全部补完：节点跳转、选项处理、effects 应用、条件表达式求值（支持 `>= <= == != > <` + `&& \|\|`，递归字符串拆分）|
| `src/story/StoryEngine.h` | 增加 `readVariable`、`evaluateAtomic`、`effectsApplied` 信号 |
| `src/story/Subjects.cpp` | 4 个学科派生类的 `interact()` 实现 |
| `src/common/Subject.h` | 增加 `interactionRequested` 信号 |
| `src/ui/MainWindow.cpp` | 接入 StoryEngine，对话页能跑剧本（**注**：C 在 game_dev 上扩了这文件 +415 行，已合并）|
| `src/ui/MainWindow.h` | 增加 `storyEngine_` / `dialogWindow_` 成员 |
| `assets/resources.qrc` | 注册全部 14 个 JSON 剧本资源 |

### 二、剧本资产（14 文件 · 237 节点 · 全部通过结构校验）

#### 周剧本

| 文件 | 节点数 | 状态 |
|---|---|---|
| `week1.json` | 31 | ✅ 完整版（穿越开场 + 第一天分支） |
| `week2.json` | 34 | ✅ 适应期 + 首次小测 + 黑化变体 |
| `week3.json` | 30 | ✅ 期中考 + 4 主攻支线 + 考后陪同分流 |
| `week4.json` | 36 | ✅ 信物 4 路 + 出游 2 目的地 × 5 陪同 |
| `week5.json` | 30 | ✅ 4 角色最后告白 + 最终选择 4 路 |

#### 角色支线

| 文件 | 节点数 | 状态 |
|---|---|---|
| `route_progdesign.json` | 8 | ✅ +hub 分流（`<60` / `>=60`）|
| `route_calculus.json` | 10 | ✅ +hub 三段分流（`<50` / `50-79` / `>=80`）|
| `route_linearalgebra.json` | 9 | ✅ +hub 按 darkness 分流；修 la_endgame 空 choices bug |
| `route_ai.json` | 12 | ✅ +hub 三段分流 |

#### 结局剧本

| 文件 | 节点数 | 状态 |
|---|---|---|
| `ending_1_good_grade.json` | 7 | ✅ 修终点空 choices |
| `ending_2_best_love.json` | 7 | ✅ 修终点空 choices；end2_leave_anyway 改为正式旁白 |
| `ending_3_farewell.json` | 8 | ✅ 修终点空 choices |
| `ending_4_eternal_la.json` | 5 | ✅ 修终点空 choices |
| `ending_5_hospital.json` | 10 | ✅ 修终点空 choices |

### 三、关键设计决策与改动

#### 1. Hub 节点 + condition 分流（route 对话池）
4 个 route 文件统一加 hub 节点做新 startNode，用 condition-gated「（继续）」选项分发到 lv1/lv2/endgame。**纯剧本侧解决，不动 C++**。

#### 2. LA 调性矫正（去 romance-coded）
按"角色不该是恋人型"原则，把所有 LA 台词改为**纯监控 + 归档语法**：
- 删：「我对你有感情」「你属于我」「我都能找到你」等情感投射台词
- 改：「你被包含在我今晚的观察范围内」「你不会从我的坐标系里出去」「这种偏差会留在记录里」
- LA 的高好感 ≠ mutual attraction，= **玩家未能逃出她的视野**

#### 3. AI 引论去强行术语
删除 5 处把日常指令里堆砌 ML 术语的台词（梯度图、损失曲面、cooldown 等），保留少数高层比喻台词（反向传播、调参、分布之外）。

### 四、测试工具（30/30 PASS）

| 文件 | 覆盖范围 |
|---|---|
| `tests/simulate_routes.py` | Python 复刻 StoryEngine 行为，跑通 5 主路径 |
| `tests/simulate_boosted.py` | hub 分流（11）+ 信物阈值（5）+ 黑化结局触发（4）+ 5 结局独立跑通（5）|

### 五、文档（给队友看 / 给助教看）

| 文件 | 用途 |
|---|---|
| [作业报告_成员A_第1-3章.md](作业报告_成员A_第1-3章.md) | 功能介绍 + 剧情系统设计 + 5 结局介绍，4 页 |
| [录屏脚本.md](录屏脚本.md) | 4 分钟分秒级演示流程 + 录前 checklist |
| [角色设定_美术参考.md](角色设定_美术参考.md) | 给 B 的 4 角色立绘 brief（配色、表情库、❌不要清单）|

### 六、环境与编译

- ✅ Qt 6.11.1 + Multimedia + qtsvg + qtdeclarative 本机可用
- ✅ `cmake --build build` 23/23 编译单元过（C 砍 2 个 minigame 后总数从 25 → 23）
- ✅ `StudyAdventure.app` 可执行文件生成成功

### 七、Git 状态（5/23 完成）

- ✅ **commit + push**：本轮所有工作打包到 `feature/story-rewrite-and-tests`，已推送
- ✅ **merge**：C 的 `origin/game_dev` 已合入 `develop` 并 push（merge commit `19d9fa2`）
- ✅ **整合**：把新 develop merge 回 A 的 feature 分支，编译 + 剧本测试全过
- ⏳ **待开 PR**：`feature/story-rewrite-and-tests` → `develop`，把 A 这一轮的剧本修复也合进主线

---

## 🚧 还剩待办

### 🔴 P0 · 立即做

| # | 任务 | 阻塞 |
|---|---|---|
| 1 | 开 PR：`feature/story-rewrite-and-tests` → `develop` | 无 |
| 2 | 跟 C 谈高数→BlackjackGame 设计冲突——他砍了 CalculusGame，导致高数现在关联到 21 点，与"积分把瞬间连成人生"的诗性人设违和 | 等 C 回复 |
| 3 | 跟 C 确认 `end2_leave_anyway` 跨剧本跳转机制——剧本里玩家走这个分支应跳到 End3，但 JSON 没有跨剧本跳转字段，需要 GameManager 显式处理 | 等 C 回复 |

### 🟡 P0 · 需要队友配合

| # | 任务 | 阻塞 |
|---|---|---|
| 4 | 真实 GUI 跑通 5 路径（不是 Python 模拟器，是启动 .app 实测）| 需要 B 的 dialog UI + C 的 GameManager 已能切换场景 |
| 5 | 检查 C 改的 MainWindow.cpp (+415 行) 是否破坏 StoryEngine 的信号连接 | 需要看 diff |

### 🟢 P1 · 跟进型

| # | 任务 | 阻塞 |
|---|---|---|
| 6 | 把 [角色设定_美术参考.md](角色设定_美术参考.md) 发给 B | 无 |
| 7 | B 立绘做完后，写 speaker → 立绘文件名映射的接口约定（短文档）| 等 B 出立绘 |
| 8 | 合并三人版本 → 作业报告终稿 | 等 B/C 提供 4-6 章 |
| 9 | 实际录制 4 分钟演示视频 | 需要三人配合 |

### 💭 P2 · 可选改进

| # | 任务 | 备注 |
|---|---|---|
| 10 | 给 `StoryEngine::loadScript` 加可选 `startNode` 参数，让 GameManager 显式分发 route 池 | 比当前 hub 方案更优雅，但要改 C++ |
| 11 | 给 `progdesign_lv1` 区间补 lv1.5 节点 | 当前 hub 把 30-59 区间送进 lv1，体验上初识对话会重看 |

---

## 📊 数据快照

```
剧本资产  ：14 文件 / 237 节点 / 0 断链 / 0 孤儿 / 0 死端
自动测试  ：30/30 通过
项目编译  ：23/23 编译单元（合 C 后）✓ 可执行文件已生成
代码贡献  ：StoryEngine.cpp (296 行) + 4 个剧本扩写 + 路径模拟器
文档贡献  ：作业报告 1-3 章 + 录屏脚本 + 角色美术参考 + 本进度文档
Git       ：feature 分支已推送；C 的 game_dev 已合入 develop；
            PR (feature → develop) 待开
```

---

## 📝 剧本写作技巧（自己用）

- 每个节点台词 1-3 句话，**不要写过长段落**（对话框装不下）
- 每周剧本 30-50 节点为佳
- 选项 2-3 个最好
- effects 数值：好感度 ±2~5、压力 ±3~10、黑化 ±2~8
- 黑化倾向的剧情用 `condition` 控制（如 `"darkness >= 30"`）
- **LA 写作铁律**：不报告情绪、不主动触碰玩家、所有"想要"改为"系统不容忍"

---

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
