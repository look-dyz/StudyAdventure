# JSON 剧本格式速查

> 给成员 A 写剧本时随手查的参考。完整版见 `接口约定.md` 第 4 节。

## 最小可用模板

```json
{
  "title": "剧本标题",
  "startNode": "node_1",
  "nodes": [
    {
      "id": "node_1",
      "speaker": "narrator",
      "text": "这里是台词。",
      "choices": [
        { "text": "选项A", "next": "node_2", "effects": [] },
        { "text": "选项B", "next": "END", "effects": [{"target": "stress", "delta": 5}] }
      ]
    },
    {
      "id": "node_2",
      "speaker": "ProgDesign",
      "text": "我是程设。",
      "choices": []
    }
  ]
}
```

## 字段速查

### speaker（说话人）
- `"narrator"` — 旁白（无立绘）
- `"player"` — 主角
- `"ProgDesign"` — 程设
- `"Calculus"` — 高数
- `"LinearAlgebra"` — 线代
- `"AIIntro"` — AI 引论

### next（下一节点）
- 普通节点 id（字符串）
- `"END"` — 剧本结束

### effects（数值变更）
```json
[
  {"target": "affinity.ProgDesign", "delta": 5},   // 程设好感 +5
  {"target": "affinity.Calculus", "delta": -3},     // 高数好感 -3
  {"target": "stress", "delta": 10},                // 压力 +10
  {"target": "darkness", "delta": 8}                // 线代黑化 +8
]
```

target 取值：
- `affinity.<学科>` — `ProgDesign` / `Calculus` / `LinearAlgebra` / `AIIntro`
- `stress` — 压力
- `darkness` — 线代黑化

### condition（条件，可选）
```json
{
  "id": "secret_node",
  "speaker": "LinearAlgebra",
  "text": "你已经……是我的人了……",
  "choices": [],
  "condition": "darkness >= 50"
}
```

支持的运算符：`<` `<=` `==` `>=` `>` `!=`
支持的变量：`affinity.<学科>`、`stress`、`darkness`、`week`（当前周）、`day`

## 写作风格建议

- **每个节点的台词不超过 2-3 句话**，否则对话框装不下要滚动。
- **选项 2-3 个为佳**，太多玩家不知道选哪个。
- **每周剧本约 30-50 个节点**，5 周下来总量约 150-250 节点。
- **结局剧本可以稍长**（10-15 个节点的演出感）。

## 常用剧本组织

每周一个文件：`week1.json` / `week2.json` / ... / `week5.json`

每条好感线一个文件：
- `route_progdesign.json`
- `route_calculus.json`
- `route_linearalgebra.json`
- `route_ai.json`

每个结局一个文件：
- `ending_1_good_grade.json`
- `ending_2_best_love.json`
- `ending_3_farewell.json`
- `ending_4_eternal_la.json`
- `ending_5_hospital.json`
