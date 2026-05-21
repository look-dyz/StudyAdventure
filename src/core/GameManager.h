/**
 * @file GameManager.h
 * @brief 游戏主控（单例模式）
 *
 * 全局唯一实例，负责：
 * - 持有 Player 对象（数值系统）
 * - 协调场景切换（主菜单 / 地图 / 对话 / 小游戏）
 * - 接收小游戏的 finished 信号并应用奖惩
 * - 触发存档读档
 *
 * 负责人：成员 C
 *
 * ===== 使用方式 =====
 * @code
 *   GameManager& mgr = GameManager::instance();
 *   mgr.player()->addAffinity(SubjectType::ProgDesign, 5);
 *   mgr.requestScene(GameScene::Map);
 * @endcode
 */
#ifndef STUDYADVENTURE_GAMEMANAGER_H
#define STUDYADVENTURE_GAMEMANAGER_H

#include <QObject>
#include "common/Constants.h"

namespace SA {

class Player;

class GameManager : public QObject {
    Q_OBJECT
public:
    /// 获取单例实例
    static GameManager& instance();

    /// 禁止拷贝
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    /// 获取玩家对象
    Player* player() const { return player_; }

    /// 请求切换场景（UI 监听后实际切换 QStackedWidget）
    void requestScene(GameScene scene);

    /// 处理小游戏结束（成员 C 实现奖惩计算）
    void onMiniGameFinished(MiniGameType type, int score, bool won);

signals:
    /// 场景切换请求（MainWindow 监听）
    void sceneChangeRequested(GameScene scene);

    /// 游戏结束（结局触发，UI 切到结局展示）
    void endingTriggered(EndingType ending);

private:
    GameManager(QObject* parent = nullptr);
    ~GameManager() = default;

    Player* player_;
};

} // namespace SA

#endif // STUDYADVENTURE_GAMEMANAGER_H
