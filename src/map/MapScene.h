/**
 * @file MapScene.h
 * @brief 地图场景（用 QGraphicsScene 派生实现热区点击）
 *
 * 负责人：成员 B
 */
#ifndef STUDYADVENTURE_MAPSCENE_H
#define STUDYADVENTURE_MAPSCENE_H

#include <QGraphicsScene>
#include "common/Constants.h"

namespace SA {

class MapScene : public QGraphicsScene {
    Q_OBJECT
public:
    explicit MapScene(QObject* parent = nullptr);

    /// 切换到指定地点场景
    void setLocation(Location loc);

signals:
    /// 玩家点击了某个地点
    void locationClicked(Location loc);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
    Location current_ = Location::Classroom;
};

} // namespace SA

#endif // STUDYADVENTURE_MAPSCENE_H
