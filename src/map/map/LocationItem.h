/**
 * @file LocationItem.h
 * @brief 地图上的可点击地点（热区）—— QGraphicsObject 派生
 *
 * 名为 LocationItem 而非 Location，避免与 SA::Location 枚举重名冲突。
 *
 * 负责人：成员 B
 */
#ifndef STUDYADVENTURE_LOCATIONITEM_H
#define STUDYADVENTURE_LOCATIONITEM_H
#include <QGraphicsObject>
#include <QString>
#include <QPixmap>          // 新增
#include "common/Constants.h"
namespace SA {
class LocationItem : public QGraphicsObject {
    Q_OBJECT
public:
    LocationItem(Location type, const QRectF& rect, const QString& label,
                 const QString& imagePath = QString(),  // 新增
                 QGraphicsItem* parent = nullptr);
    QRectF boundingRect() const override { return rect_; }
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;
    Location locationType() const { return type_; }
signals:
    void clicked(Location type);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
private:
    Location type_;
    QRectF rect_;
    QString label_;
    bool hovered_ = false;
    QPixmap bgPixmap_;      // 新增：地点背景图
};
} // namespace SA
#endif // STUDYADVENTURE_LOCATIONITEM_H
