#include "GameManager.h"
#include "Player.h"
#include <QDebug>
#include <QMessageBox>
#include "story/EndingJudge.h"
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

        msg += QString("%1好感 +%2\n")
                   .arg(affinityName)
                   .arg(affinityDelta);
    }

    // 压力变化
    if(stressDelta != 0) {

        msg += QString("压力 +%1\n")
                   .arg(stressDelta);
    }

    // 黑化变化
    if(darknessDelta != 0) {

        msg += QString("线代黑化 +%1\n")
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

    // ← 新增：完成一局游戏消耗一次自由天数
    player_->consumeFreeDay();

    // 压力值过高触发警告
    // 原来的两段触发代码，改成：
    if (player_->stress() >= 100) {
        qDebug() << "[GameManager] Stress reached maximum!";
        currentEnding_ = EndingJudge::judge(player_, player_->choseToStay());
        // stress >= 100 时 judge() 优先级1会返回 End5，无需手动指定
        requestScene(GameScene::Ending);
        return;
    }

    if (player_->darkness() >= 100) {
        qDebug() << "[GameManager] Darkness triggered ending";
        currentEnding_ = EndingJudge::judge(player_, player_->choseToStay());
        // darkness >= 100 且 stress < 100 时，judge() 优先级2返回 End4
        requestScene(GameScene::Ending);
        return;
    }
    // 玩完小游戏后默认回到地图场景
    requestScene(GameScene::Map);
}

void GameManager::onStoryFinished() {
    currentEnding_ = EndingJudge::judge(player_, player_->choseToStay());
    requestScene(GameScene::Ending);
}

} // namespace SA
