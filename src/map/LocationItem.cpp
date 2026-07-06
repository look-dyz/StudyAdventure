#include "LocationItem.h"
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QCursor>

namespace SA {

LocationItem::LocationItem(Location type,
                           const QRectF& rect,
                           const QString& label,
                           QGraphicsItem* parent)
    : QGraphicsObject(parent), type_(type), rect_(rect), label_(label)
{
    setAcceptHoverEvents(true);
    setCursor(Qt::PointingHandCursor);
}

void LocationItem::paint(QPainter* painter,
                         const QStyleOptionGraphicsItem*,
                         QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);

            // 背景：悬停时暖橙，正常时深蓝半透明
    QColor bgColor = hovered_
                         ? QColor(200, 100, 40, 210)
                         : QColor(30, 40, 80, 180);

    QPainterPath path;
    path.addRoundedRect(rect_, 12, 12);
    painter->fillPath(path, bgColor);

            // 边框
    QPen borderPen(hovered_ ? QColor("#FFD580") : QColor("#8AAED6"), 2);
    painter->setPen(borderPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRoundedRect(rect_, 12, 12);

            // 文字
    QFont font = painter->font();
    font.setPointSize(14);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(hovered_ ? QColor("#FFD580") : Qt::white);
    painter->drawText(rect_, Qt::AlignCenter, label_);

            // 悬停时底部提示
    if (hovered_) {
        QFont hintFont = painter->font();
        hintFont.setPointSize(9);
        hintFont.setBold(false);
        painter->setFont(hintFont);
        painter->setPen(QColor(255, 220, 150, 200));
        QRectF hintRect(rect_.left(), rect_.bottom() - 20,
                        rect_.width(), 20);
        painter->drawText(hintRect, Qt::AlignCenter, "点击进入");
    }
}

void LocationItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::LeftButton)
        emit clicked(type_);
    QGraphicsObject::mousePressEvent(event);
}
void LocationItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    hovered_ = true;  update();
    QGraphicsObject::hoverEnterEvent(event);
}
void LocationItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    hovered_ = false; update();
    QGraphicsObject::hoverLeaveEvent(event);
}

} // namespace SA