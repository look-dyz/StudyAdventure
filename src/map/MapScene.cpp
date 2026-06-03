#include "MapScene.h"
#include "LocationItem.h"
#include <QGraphicsSimpleTextItem>
#include <QGraphicsPixmapItem>  // 新增
#include <QFont>
#include <QBrush>
#include <QPen>
#include <QPixmap>              // 新增
namespace SA {
MapScene::MapScene(QObject* parent) : QGraphicsScene(parent) {
    setSceneRect(0, 0, 1024, 600);

            // ── 新增：大地图背景图 ────────────────────────────────────
    QPixmap mapBg(":/images/ui/大地图.jpg");
    if (!mapBg.isNull()) {
        auto* bgItem = addPixmap(
            mapBg.scaled(1024, 600,
                         Qt::KeepAspectRatioByExpanding,
                         Qt::SmoothTransformation));
        bgItem->setPos(0, 0);
        bgItem->setZValue(-1); // 最底层
    } else {
        // 原有 fallback
        setBackgroundBrush(QBrush(QColor("#F5F0EC")));
    }
    // ─────────────────────────────────────────────────────────

            // 原有：标题
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
        QString bgImage;    // 新增
    };
    const QList<LocDef> defs = {
        // 原有位置不变，新增 bgImage 字段，按名字对应图片
        { Location::Classroom,
         QRectF(80,  100, 220, 140),
         "教学楼\n📚 上课·逃课",
         ":/images/backgrounds/理教.png"     },  // 理教 → Classroom
        { Location::Library,
         QRectF(380, 100, 220, 140),
         "图书馆\n📖 自习·小游戏",
         ":/images/backgrounds/图书馆.png"   },  // 图书馆 → Library
        { Location::WeimingLake,
         QRectF(680, 100, 220, 140),
         "未名湖\n🌊 散步·哲学",
         ":/images/backgrounds/未名湖.png"   },  // 未名湖 → WeimingLake
        { Location::Dormitory,
         QRectF(380, 340, 220, 140),
         "宿舍\n🛏 休息·减压",
         ":/images/backgrounds/宿舍.png"     },  // 宿舍 → Dormitory
    };
    for (const auto& d : defs) {
        // 原有构造调用，新增传入 bgImage
        auto* loc = new LocationItem(d.type, d.rect, d.label, d.bgImage);
        connect(loc, &LocationItem::clicked, this, &MapScene::locationClicked);
        addItem(loc);
        locations_.append(loc);
    }
}
} // namespace SA
