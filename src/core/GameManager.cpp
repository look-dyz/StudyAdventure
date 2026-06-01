#include "GameManager.h"
#include "Player.h"
#include <QDebug>
#include <QMessageBox>
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

    int affinityDelta = 0;
    int stressDelta = 0;
    int darknessDelta = 0;

    QString affinityName;

    switch(type) {

        // ==============================
        // 扫雷（程序设计）
        // ==============================
        case MiniGameType::Minesweeper:

            affinityName = "程序设计";

            if(won) {

                affinityDelta = 10;
                stressDelta = 5;

                player_->addAffinity(
                    SubjectType::ProgDesign,
                    affinityDelta
                    );

                player_->addStress(stressDelta);

            } else {

                stressDelta = 15;

                player_->addStress(stressDelta);
            }

            break;

                    // ==============================
                    // 记忆翻牌（线代）
                    // ==============================
        case MiniGameType::MemoryMatch:

            affinityName = "线性代数";

            if(won) {

                affinityDelta = 10;
                stressDelta = 5;

                player_->addAffinity(
                    SubjectType::LinearAlgebra,
                    affinityDelta
                    );

                player_->addStress(stressDelta);

            } else {

                stressDelta = 12;
                darknessDelta = 15;

                player_->addStress(stressDelta);

                player_->addDarkness(darknessDelta);
            }

            break;

                    // ==============================
                    // AI迷宫（AI引论）
                    // ==============================
        case MiniGameType::Maze:

            affinityName = "AI引论";

            if(won) {

                affinityDelta = 10;
                stressDelta = 5;

                player_->addAffinity(
                    SubjectType::AIIntro,
                    affinityDelta
                    );

                player_->addStress(stressDelta);

            } else {

                stressDelta = 14;

                player_->addStress(stressDelta);
            }

            break;

                    // ==============================
                    // 21点（高数）
                    // ==============================
        case MiniGameType::Blackjack:

            affinityName = "高等数学";

            if(won) {

                affinityDelta = 5;
                stressDelta = 2;

                player_->addAffinity(
                    SubjectType::Calculus,
                    affinityDelta
                    );

                player_->addStress(stressDelta);

            } else {

                stressDelta = 5;

                player_->addStress(stressDelta);
            }

            break;

                    // ==============================
                    // 井字棋（程序设计）
                    // ==============================
        case MiniGameType::TicTacToe:

            affinityName = "程序设计";

            if(won) {

                affinityDelta = 10;

                player_->addAffinity(
                    SubjectType::ProgDesign,
                    affinityDelta
                    );

            } else {

                stressDelta = 10;

                player_->addStress(stressDelta);
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

        stressDelta -= 3;

        player_->addStress(-3);

        qDebug() << "[GameManager] Bonus reward for high score!";
    }

    // ==============================
    // 数值变化提示框
    // ==============================

    QString msg;

    // 好感变化
    if(affinityDelta != 0) {

        msg += QString("%1好感 %+2\n")
                   .arg(affinityName)
                   .arg(affinityDelta);
    }

    // 压力变化
    if(stressDelta != 0) {

        msg += QString("压力 %+1\n")
                   .arg(stressDelta);
    }

    // 黑化变化
    if(darknessDelta != 0) {

        msg += QString("线代黑化 %+1\n")
                   .arg(darknessDelta);
    }

    // 得分显示
    msg += QString("\n本次得分：%1")
               .arg(score);

    QMessageBox::information(
        nullptr,
        "数值变化",
        msg
        );

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
