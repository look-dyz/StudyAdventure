#include "StoryEngine.h"
#include "core/Player.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

namespace SA {

StoryEngine::StoryEngine(Player* player, QObject* parent)
    : QObject(parent), player_(player) {
}

bool StoryEngine::loadScript(const QString& scriptPath) {
    QFile file(scriptPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[StoryEngine] Cannot open script:" << scriptPath;
        return false;
    }

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "[StoryEngine] JSON parse error:" << err.errorString();
        return false;
    }

    scriptRoot_ = doc.object();
    qDebug() << "[StoryEngine] Loaded script:" << scriptPath;

    // 跳转到 startNode（如果存在）
    QString startId = scriptRoot_.value("startNode").toString("start");
    jumpToNode(startId);
    return true;
}

void StoryEngine::jumpToNode(const QString& nodeId) {
    // TODO（成员 A）：实现节点跳转逻辑
    // 1. 在 scriptRoot_["nodes"] 中查找 id == nodeId 的节点
    // 2. 检查 condition（如果有），不满足则跳过
    // 3. 应用 effects（数值变更）
    // 4. 设置 currentNodeId_ 并发出 nodeChanged 信号
    currentNodeId_ = nodeId;
    qDebug() << "[StoryEngine] Jump to node:" << nodeId;
    emit nodeChanged();
}

void StoryEngine::onChoiceSelected(int choiceIndex) {
    // TODO（成员 A）：实现选项处理
    // 1. 从当前节点的 choices 数组中取第 choiceIndex 个选项
    // 2. 应用该选项的 effects
    // 3. 跳转到 choice.next 指定的节点
    qDebug() << "[StoryEngine] Choice selected:" << choiceIndex;
}

QString StoryEngine::currentSpeaker() const {
    // TODO（成员 A）：从当前节点提取 speaker 字段
    return QString();
}

QString StoryEngine::currentText() const {
    // TODO（成员 A）：从当前节点提取 text 字段
    return QString();
}

QStringList StoryEngine::currentChoices() const {
    // TODO（成员 A）：从当前节点 choices 数组提取每个选项的 text
    return QStringList();
}

void StoryEngine::applyEffects(const QJsonObject& node) {
    // TODO（成员 A）：解析 effects 数组，调用 player_ 的方法修改数值
    // 示例 effects 格式：
    //   "effects": [
    //     {"target": "affinity.ProgDesign", "delta": 5},
    //     {"target": "stress", "delta": -3}
    //   ]
    Q_UNUSED(node);
}

bool StoryEngine::evaluateCondition(const QString& expression) const {
    // TODO（成员 A）：实现简单的条件表达式求值
    // 示例：
    //   "affinity.LinearAlgebra >= 50"
    //   "stress < 80"
    Q_UNUSED(expression);
    return true;
}

} // namespace SA
