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
    /// 点击地点后先预览（带背景图路径和描述文字）
    void locationPreview(Location loc,
                         const QString& imagePath,
                         const QString& description);
private:
    QList<LocationItem*> locations_;
    void buildLocations();
};
} // namespace SA
#endif