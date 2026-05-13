#include "LocationItem.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QCursor>

namespace SA {

LocationItem::LocationItem(Location type, const QRectF& rect,
                           const QString& label, QGraphicsItem* parent)
    : QGraphicsObject(parent), type_(type), rect_(rect), label_(label)
{
    setAcceptHoverEvents(true);
    setCursor(Qt::PointingHandCursor);
}

void LocationItem::paint(QPainter* painter,
                         const QStyleOptionGraphicsItem* option,
                         QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QColor fill = hovered_ ? QColor(255, 200, 100, 160)
                            : QColor(100, 150, 200, 100);
    QColor border = hovered_ ? QColor("#8B1A1A") : QColor("#3B5BA5");

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(fill);
    painter->setPen(QPen(border, 2));
    painter->drawRoundedRect(rect_, 8, 8);

    painter->setPen(Qt::black);
    QFont font = painter->font();
    font.setPointSize(12);
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(rect_, Qt::AlignCenter, label_);
}

void LocationItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked(type_);
    }
    QGraphicsObject::mousePressEvent(event);
}

void LocationItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    hovered_ = true;
    update();
    QGraphicsObject::hoverEnterEvent(event);
}

void LocationItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    hovered_ = false;
    update();
    QGraphicsObject::hoverLeaveEvent(event);
}

} // namespace SA
