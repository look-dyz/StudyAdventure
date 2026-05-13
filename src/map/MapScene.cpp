#include "MapScene.h"
#include "LocationItem.h"
#include <QGraphicsSimpleTextItem>
#include <QFont>
#include <QBrush>
#include <QPen>

namespace SA {

MapScene::MapScene(QObject* parent) : QGraphicsScene(parent) {
    setSceneRect(0, 0, 1024, 600);
    setBackgroundBrush(QBrush(QColor("#F5F0EC")));

    // 装饰：在背景上添加一个标题
    auto* title = addSimpleText("北大校园 · 异世界版");
    QFont titleFont = title->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setBrush(QBrush(QColor("#8B1A1A")));
    title->setPos(380, 20);

    buildLocations();
}

void MapScene::buildLocations() {
    struct LocDef {
        Location type;
        QRectF rect;
        QString label;
    };

    const QList<LocDef> defs = {
        { Location::Classroom,   QRectF(80,  100, 220, 140), "教学楼\n📚 上课·逃课" },
        { Location::Library,     QRectF(380, 100, 220, 140), "图书馆\n📖 自习·小游戏" },
        { Location::WeimingLake, QRectF(680, 100, 220, 140), "未名湖\n🌊 散步·哲学" },
        { Location::Dormitory,   QRectF(380, 340, 220, 140), "宿舍\n🛏 休息·减压" },
    };

    for (const auto& d : defs) {
        auto* loc = new LocationItem(d.type, d.rect, d.label);
        connect(loc, &LocationItem::clicked, this, &MapScene::locationClicked);
        addItem(loc);
        locations_.append(loc);
    }
}

} // namespace SA
