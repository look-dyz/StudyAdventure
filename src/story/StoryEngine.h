/**
 * @file StoryEngine.h
 * @brief 剧情引擎（读取 JSON 剧本、推进剧情、处理分支）
 *
 * 负责人：成员 A
 *
 * ===== JSON 剧本格式（详见 docs/接口约定.md）=====
 * 每个剧本是一个节点列表，节点包含：
 *   - id: 节点唯一标识
 *   - speaker: 说话人（学科类型或 "narrator"）
 *   - text: 台词
 *   - choices: 选项列表（每个选项含 text、next、effects）
 *   - effects: 数值变更指令（如 "affinity.ProgDesign+=5"）
 *   - condition: 显示条件（可选）
 */
#ifndef STUDYADVENTURE_STORYENGINE_H
#define STUDYADVENTURE_STORYENGINE_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include "common/Constants.h"

namespace SA {

class Player;

class StoryEngine : public QObject {
    Q_OBJECT
public:
    explicit StoryEngine(Player* player, QObject* parent = nullptr);

    /// 加载剧本文件
    /// @param scriptPath 剧本 JSON 文件路径（如 ":/scripts/week1.json"）
    /// @return 成功返回 true
    bool loadScript(const QString& scriptPath);

    /// 跳转到指定节点
    void jumpToNode(const QString& nodeId);

    /// 玩家选择了某个选项
    void onChoiceSelected(int choiceIndex);

    /// 当前节点信息（供 UI 显示）
    QString currentSpeaker() const;
    QString currentText() const;
    QStringList currentChoices() const;

    /// 当前节点 ID
    QString currentNodeId() const { return currentNodeId_; }

signals:
    /// 节点切换信号（UI 监听后刷新对话框）
    void nodeChanged();

    /// 剧本结束信号
    void scriptFinished();

private:
    Player* player_;            ///< 玩家对象指针（不持有，外部生命周期管理）
    QJsonObject scriptRoot_;    ///< 当前加载的剧本根对象
    QString currentNodeId_;     ///< 当前节点 ID

    /// 应用节点的 effects（数值变更指令）
    void applyEffects(const QJsonObject& node);

    /// 评估条件表达式
    bool evaluateCondition(const QString& expression) const;
};

} // namespace SA

#endif // STUDYADVENTURE_STORYENGINE_H
