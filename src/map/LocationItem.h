#ifndef STUDYADVENTURE_LOCATIONITEM_H
#define STUDYADVENTURE_LOCATIONITEM_H
#include <QGraphicsObject>
#include <QString>
#include "common/Constants.h"

namespace SA {
class LocationItem : public QGraphicsObject {
    Q_OBJECT
public:
    LocationItem(Location type,
                 const QRectF& rect,
                 const QString& label,
                 QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override { return rect_; }
    void   paint(QPainter* painter,
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
    QRectF   rect_;
    QString  label_;
    bool     hovered_ = false;
};
} // namespace SA
#endif