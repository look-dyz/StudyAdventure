#include "MapScene.h"
#include "LocationItem.h"
#include <QGraphicsSimpleTextItem>
#include <QGraphicsPixmapItem>
#include <QFont>
#include <QBrush>
#include <QPen>
#include <QPixmap>

namespace SA {

MapScene::MapScene(QObject* parent) : QGraphicsScene(parent) {
    setSceneRect(0, 0, 1024, 600);

    QPixmap mapBg(":/images/ui/map.jpg");
    if (!mapBg.isNull()) {
        auto* bgItem = addPixmap(
            mapBg.scaled(1024, 600,
                         Qt::KeepAspectRatioByExpanding,
                         Qt::SmoothTransformation));
        bgItem->setPos(0, 0);
        bgItem->setZValue(-1);
    } else {
        setBackgroundBrush(QBrush(QColor("#F5F0EC")));
    }

    auto* title = addSimpleText("北大校园 · 异世界版");
    QFont titleFont = title->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setBrush(QBrush(QColor("#FFFFFF")));
    title->setPos(370, 20);

    buildLocations();
}

void MapScene::buildLocations() {
    struct LocDef {
        Location    type;
        QRectF      rect;
        QString     label;       // 文字框显示文字
        QString     imagePath;   // 详情页背景图
        QString     description; // 详情页剧情框文字
    };

    const QList<LocDef> defs = {
                                { Location::Classroom,
                                 QRectF(80,  120, 200, 100),
                                 "📚  教学楼",
                                 ":/images/backgrounds/classroom.png",
                                 "教学楼 · 理教\n"
                                 "\n"
                                 "这里是上课的地方。\n"
                                 "进入后将触发本周课程剧情，\n"
                                 "或与学科角色进行互动。" },

                                { Location::Library,
                                 QRectF(390, 120, 200, 100),
                                 "📖  图书馆",
                                 ":/images/backgrounds/library.png",
                                 "图书馆\n"
                                 "\n"
                                 "安静的自习室散发着书香。\n"
                                 "这里可以玩小游戏来提升学科好感，\n"
                                 "消耗一天自由活动时间。" },

                                { Location::WeimingLake,
                                 QRectF(700, 120, 200, 100),
                                 "🌊  未名湖",
                                 ":/images/backgrounds/lake.png",
                                 "未名湖畔\n"
                                 "\n"
                                 "湖面波光粼粼，微风轻拂。\n"
                                 "在这里散步可以缓解压力 -5，\n"
                                 "消耗一天自由活动时间。" },

                                { Location::Dormitory,
                                 QRectF(390, 360, 200, 100),
                                 "🛏  宿舍",
                                 ":/images/backgrounds/dormitory.png",
                                 "学生宿舍\n"
                                 "\n"
                                 "回到宿舍好好休息。\n"
                                 "压力 -10，时间推进一天，\n"
                                 "消耗一天自由活动时间。" },
                                };
    for (const auto& d : defs) {
        auto* loc = new LocationItem(d.type, d.rect, d.label);
        // 点击后发预览信号，不直接触发地点逻辑
        connect(loc, &LocationItem::clicked, this,
                [this, d](Location type) {
                    emit locationPreview(type, d.imagePath, d.description);
                });
        addItem(loc);
        locations_.append(loc);
    }
}

} // namespace SA