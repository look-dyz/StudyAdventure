# assets/ — 资源文件目录

## 目录说明

- `images/characters/` — 4 个学科角色的立绘（每人至少 3 个表情：normal、happy、sad）
- `images/backgrounds/` — 4 张地图背景图（教学楼、图书馆、未名湖、宿舍）
- `images/ui/` — UI 元素（按钮、对话框边框、进度条样式等）
- `audio/bgm/` — 背景音乐（建议 4-6 首，覆盖不同情境）
- `audio/sfx/` — 音效（点击、翻页、胜利、失败等）
- `scripts/` — JSON 剧本文件（由成员 A 维护）
- `data/` — 题库等数据文件（由成员 C 维护）

## 命名规范

```
images/characters/progdesign_normal.png
images/characters/progdesign_happy.png
images/backgrounds/classroom.png
audio/bgm/morning.mp3
audio/sfx/click.wav
scripts/week1.json
scripts/ending_best_love.json
data/calculus_questions.json
```

## 素材来源建议

- **图片**：Pixabay、Unsplash（背景）；Pixiv 二次创作素材（注意版权）；AI 生图工具
- **音乐**：爱给网（aigei.com）、freesound.org（注意 CC 协议）
- **字体**：思源系列（开源）、Noto 系列

## 资源整合（Qt 资源系统）

完成素材收集后，创建 `assets/resources.qrc`：

```xml
<RCC>
    <qresource prefix="/">
        <file>images/backgrounds/classroom.png</file>
        <file>images/characters/progdesign_normal.png</file>
        <file>scripts/week1.json</file>
        <!-- ... 其余资源 -->
    </qresource>
</RCC>
```

然后在 `CMakeLists.txt` 中启用 `set(PROJECT_RESOURCES assets/resources.qrc)`。
