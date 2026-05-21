#include "GameManager.h"
#include "Player.h"
#include <QDebug>

namespace SA {

GameManager& GameManager::instance() {
    static GameManager s_instance;
    return s_instance;
}

GameManager::GameManager(QObject* parent)
    : QObject(parent), player_(new Player(this)) {
}

void GameManager::requestScene(GameScene scene) {
    qDebug() << "[GameManager] Requesting scene change to:" << static_cast<int>(scene);
    emit sceneChangeRequested(scene);
}

void GameManager::onMiniGameFinished(MiniGameType type, int score, bool won) {
    qDebug() << "[GameManager] MiniGame finished:"
             << "type=" << static_cast<int>(type)
             << "score=" << score
             << "won=" << won;

    // TODO（成员 C）：根据 type 和结果计算好感度/压力/黑化的变化
    // 例如：
    //   if (type == MiniGameType::Minesweeper && won) {
    //       player_->addAffinity(SubjectType::ProgDesign, 5);
    //       player_->addStress(3);
    //   }

    // 玩完小游戏后默认回到地图场景
    requestScene(GameScene::Map);
}

} // namespace SA
