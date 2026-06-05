# 贡献指南

> 给三人小组的协作规则。新成员加入也请先读完这份。

## 🌳 分支策略

```
main         ← 稳定版（仅里程碑合入：5/17, 5/27, 5/30, 6/06）
  ↑
develop      ← 默认开发分支（所有 feature 合入这里）
  ↑
feature/*    ← 个人功能分支
```

**分支命名**：
- `feature/story-*` — 成员 A 的功能
- `feature/ui-*` 或 `feature/map-*` — 成员 B 的功能
- `feature/game-*` 或 `feature/core-*` — 成员 C 的功能
- `hotfix/*` — 紧急修复（集成测试期间）
- `docs/*` — 仅文档改动

## 📝 提交规范

格式：`[模块] 简短描述`

| ✅ 好的 commit | ❌ 不好的 commit |
|---|---|
| `[story] 实现节点跳转逻辑` | `update` |
| `[ui] 修复地图点击响应` | `修了个 bug` |
| `[game] 扫雷加入首次保护机制` | `完善代码` |
| `[contract] 修改 effects 字段格式` | `change file` |

**模块标签**：`[story]` `[ui]` `[map]` `[game]` `[core]` `[common]` `[docs]` `[ci]` `[contract]`

## 🔄 标准工作流

每天开工：

```bash
# 1. 切回 develop 拉最新代码
git checkout develop
git pull

# 2. 切到自己的 feature 分支（首次创建）
git checkout -b feature/story-week1
# 或继续昨天的工作
git checkout feature/story-week1
git merge develop  # 把 develop 上的新东西合进来

# 3. 写代码……

# 4. 提交
git add src/story/StoryEngine.cpp
git commit -m "[story] 实现 jumpToNode 节点跳转"

# 5. 推送
git push origin feature/story-week1
```

完成一个功能：

```bash
# 1. 确认所有更改已 commit
git status

# 2. 在 GitHub 上发起 Pull Request → 目标分支 develop
# 3. @ 另一位队友 review
# 4. Review 通过后合并（建议用 Squash and merge 保持历史整洁）
# 5. 删除本地分支
git checkout develop
git branch -d feature/story-week1
```

## 🎯 文件归属规则

**核心原则**：你只能写自己的目录，读他人代码 OK，但**不通过 #include 内部头文件**。

| 目录 | 谁可以写入 | 跨模块通信方式 |
|---|---|---|
| `src/common/` | 任何人，但必须群里通告 | — |
| `src/core/` | 成员 C | 通过 Player 公开接口、信号槽 |
| `src/story/` | 成员 A | 通过 JSON 数据、StoryEngine 信号 |
| `src/ui/` | 成员 B | 通过 Qt 信号槽 |
| `src/map/` | 成员 B | 通过 MapScene 的 locationClicked 信号 |
| `src/games/` | 成员 C | 通过 MiniGame::finished 信号 |
| `assets/scripts/` | 成员 A | — |
| `assets/images/` | 成员 B | — |
| `assets/audio/` | 成员 B | — |
| `assets/data/` | 成员 C | — |

## 🔍 PR Review 检查清单

收到 PR review 邀请时，过一遍这些：

- [ ] 代码符合 `src/<负责模块>` 的归属规则
- [ ] commit 信息符合 `[模块] xxx` 格式
- [ ] 新增的 .h/.cpp 已加入 `CMakeLists.txt`
- [ ] 没有引入对其他模块**内部头文件**的 #include
- [ ] 接口变更已同步到 `docs/接口约定.md`
- [ ] CI 编译通过（GitHub Actions 绿色 ✓）
- [ ] 如果改了 `src/common/`，确认在群里通告过

## ⏰ 评审时效

- **小 PR（< 50 行）**：4 小时内 review
- **中等 PR（50-200 行）**：12 小时内 review
- **大 PR（> 200 行）**：1 天内 review；**强烈建议拆成多个小 PR**

## 🚨 紧急修复流程

集成测试期（5/28 起）发现严重 bug：

```bash
git checkout main
git checkout -b hotfix/crash-on-startup
# 修复
git commit -m "[core] 修复启动时空指针崩溃"
git push origin hotfix/crash-on-startup
# 直接 PR 到 main 和 develop 两个分支
```

## 💬 沟通约定

- **技术细节**：GitHub Issue/PR 评论
- **快速对齐**：微信群文字
- **复杂讨论**：周中/周末线下会
- **决策记录**：写进 `docs/` 对应文档

---

> 协作的本质是**降低对方的认知负担**。
> 写清楚 commit 信息、写清楚 PR 描述、不偷偷改别人代码、有疑问及时问——
> 这些都是在为团队节省时间。
