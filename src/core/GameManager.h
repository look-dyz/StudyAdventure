#pragma once

#include <QObject>

#include "core/Player.h"
#include "common/Constants.h"

namespace SA {

class GameManager : public QObject
{
    Q_OBJECT

public:
    static GameManager& instance();

    Player& getPlayer();

            // 场景切换
    void requestScene(GameScene scene);

            // 小游戏结束处理
    void onMiniGameFinished(MiniGameType type, int score, bool won);

signals:
    void sceneChangeRequested(GameScene scene);

private:
    explicit GameManager(QObject* parent = nullptr);

    Player player_;
};

} // namespace SA