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
    switch(type) {

        // ==============================
        // 扫雷（程序设计）
        // ==============================
        case MiniGameType::Minesweeper:

            if(won) {
                player_->addAffinity(SubjectType::ProgDesign, 5);
                player_->addStress(2);
            } else {
                player_->addStress(6);
            }

            break;

                    // ==============================
                    // 高数答题（高数）
                    // ==============================
        case MiniGameType::Calculus:

            if(won) {
                player_->addAffinity(SubjectType::Calculus, 6);
                player_->addStress(4);
            } else {
                player_->addStress(8);
            }

            break;

                    // ==============================
                    // 矩阵运算（线代）
                    // ==============================
        case MiniGameType::Matrix:

            if(won) {

                player_->addAffinity(
                    SubjectType::LinearAlgebra,
                    5
                    );

            } else {

                player_->addStress(5);
                player_->addDarkness(2);
            }

            break;

                    // ==============================
                    // AI迷宫（英语/AI）
                    // ==============================
        case MiniGameType::Maze:

            if(won) {
                player_->addAffinity(SubjectType::AIIntro, 4);
                player_->addStress(3);
            } else {
                player_->addStress(7);
            }

            break;

                    // ==============================
                    // 21点
                    // ==============================
        case MiniGameType::Blackjack:

            if(won) {
                player_->addAffinity(SubjectType::ProgDesign, 3);
            } else {
                player_->addStress(4);
            }

            break;

                    // ==============================
                    // 井字棋
                    // ==============================
        case MiniGameType::TicTacToe:

            if(won) {
                player_->addAffinity(SubjectType::ProgDesign, 2);
            } else {
                player_->addStress(2);
            }

            break;

        default:
            qDebug() << "[GameManager] Unknown MiniGameType";
            break;
    }

            // ==============================
            // 通用奖励逻辑
            // ==============================

            // 高分额外奖励
    if(score >= 95) {
        player_->addStress(-3);

        qDebug() << "[GameManager] Bonus reward for high score!";
    }

            // 压力值过高触发警告
    if(player_->stress() >= 100) {

        qDebug() << "[GameManager] Stress reached maximum!";

                // TODO：
                // 后续可以：
                // 1. 触发 Bad Ending
                // 2. 强制休息
                // 3. Game Over

                // emit endingTriggered(...);
    }
    // 玩完小游戏后默认回到地图场景
    requestScene(GameScene::Map);
}

} // namespace SA
