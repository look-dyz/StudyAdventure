#include "LocationItem.h"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QCursor>
namespace SA {
LocationItem::LocationItem(Location type, const QRectF& rect,
                           const QString& label,
                           const QString& imagePath,   // 新增
                           QGraphicsItem* parent)
    : QGraphicsObject(parent), type_(type), rect_(rect), label_(label)
{
    setAcceptHoverEvents(true);
    setCursor(Qt::PointingHandCursor);
    // 新增：加载背景图
    if (!imagePath.isEmpty())
        bgPixmap_ = QPixmap(imagePath);
}

void LocationItem::paint(QPainter* painter,
                         const QStyleOptionGraphicsItem* option,
                         QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setRenderHint(QPainter::SmoothPixmapTransform); // 新增

            // ── 新增：圆角裁剪，在区域内绘制背景图 ──────────────────
    QPainterPath clipPath;
    clipPath.addRoundedRect(rect_, 8, 8);
    painter->save();
    painter->setClipPath(clipPath);
    if (!bgPixmap_.isNull()) {
        painter->drawPixmap(
            rect_.toRect(),
            bgPixmap_.scaled(rect_.size().toSize(),
                             Qt::KeepAspectRatioByExpanding,
                             Qt::SmoothTransformation));
    } else {
        // 原有 fallback 纯色
        QColor fill = hovered_ ? QColor(255, 200, 100, 160)
                               : QColor(100, 150, 200, 100);
        painter->fillRect(rect_, fill);
    }
    painter->restore();
    // ────────────────────────────────────────────────────────

            // 原有：hover 半透明遮罩（保留原逻辑，图片上叠加）
    if (hovered_) {
        QPainterPath p;
        p.addRoundedRect(rect_, 8, 8);
        painter->fillPath(p, QColor(255, 200, 100, 80));
    }

            // 原有：边框
    QColor border = hovered_ ? QColor("#8B1A1A") : QColor("#3B5BA5");
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(border, 2));
    painter->drawRoundedRect(rect_, 8, 8);

            // 原有：文字（加深底色保证可读性）
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0, 0, 0, 120)); // 新增：文字区半透明底
    QRectF textBg(rect_.left(), rect_.bottom() - 52, rect_.width(), 52);
    painter->drawRoundedRect(textBg, 0, 0);

    painter->setPen(Qt::white);              // 原白字（原为 Qt::black，改白更清晰）
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
