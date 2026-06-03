/**
 * @file MapScene.h
 * @brief 地图场景：管理 4 个 LocationItem 热区
 *
 * 用法：放入 QGraphicsView 显示。
 * LocationItem 点击事件会被转发为 locationClicked 信号。
 *
 * 负责人：成员 B
 */
#ifndef STUDYADVENTURE_MAPSCENE_H
#define STUDYADVENTURE_MAPSCENE_H

#include <QGraphicsScene>
#include <QList>
#include "common/Constants.h"

namespace SA {

class LocationItem;

class MapScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit MapScene(QObject* parent = nullptr);

signals:
    /// 任一 LocationItem 被点击时转发
    void locationClicked(Location loc);

private:
    QList<LocationItem*> locations_;

    void buildLocations();
};

} // namespace SA

#endif // STUDYADVENTURE_MAPSCENE_H
