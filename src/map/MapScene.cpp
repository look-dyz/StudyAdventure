#include "MapScene.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

namespace SA {

MapScene::MapScene(QObject* parent) : QGraphicsScene(parent) {
    setSceneRect(0, 0, 1024, 768);
    // TODO（成员 B）：在场景中添加背景图、可点击的地点热区
}

void MapScene::setLocation(Location loc) {
    current_ = loc;
    qDebug() << "[MapScene] Switch to:" << locationName(loc);
    // TODO（成员 B）：根据 loc 切换背景图、刷新热区
}

void MapScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    // TODO（成员 B）：检测点击位置是否在某个热区内，发出 locationClicked 信号
    QGraphicsScene::mousePressEvent(event);
}

} // namespace SA
