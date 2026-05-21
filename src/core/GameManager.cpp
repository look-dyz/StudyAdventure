#include "GameManager.h"

namespace SA {

GameManager::GameManager(QObject* parent)
    : QObject(parent)
{
}

GameManager& GameManager::instance()
{
    static GameManager instance;
    return instance;
}

Player& GameManager::getPlayer()
{
    return player_;
}

void GameManager::requestScene(GameScene scene)
{
    emit sceneChangeRequested(scene);
}

void GameManager::onMiniGameFinished(MiniGameType type,
                                     int score,
                                     bool won)
{
    Q_UNUSED(type);

    if (won) {
        player_.addKnowledge(score / 10);
        player_.addEnergy(-5);
    } else {
        player_.addStress(10);
    }

    requestScene(GameScene::Map);
}

} // namespace SA