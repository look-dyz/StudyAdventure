#include "StoryEngine.h"
#include "core/Player.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QRegularExpression>

namespace SA {

StoryEngine::StoryEngine(Player* player, QObject* parent)
    : QObject(parent), player_(player) {
}

// ============================================================
// 加载剧本
// ============================================================
bool StoryEngine::loadScript(const QString& scriptPath) {
    QFile file(scriptPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[StoryEngine] Cannot open script:" << scriptPath;
        return false;
    }

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "[StoryEngine] JSON parse error in" << scriptPath
                   << ":" << err.errorString();
        return false;
    }

    scriptRoot_ = doc.object();
    qDebug() << "[StoryEngine] Loaded script:" << scriptPath
             << " title:" << scriptRoot_.value("title").toString();

    // 跳转到 startNode
    QString startId = scriptRoot_.value("startNode").toString("start");
    jumpToNode(startId);
    return true;
}

// ============================================================
// 内部辅助：根据 id 在 nodes 数组中查找节点
// ============================================================
static QJsonObject findNode(const QJsonObject& root, const QString& id) {
    QJsonArray nodes = root.value("nodes").toArray();
    for (const auto& v : nodes) {
        QJsonObject obj = v.toObject();
        if (obj.value("id").toString() == id) {
            return obj;
        }
    }
    return QJsonObject{};
}

// ============================================================
// 跳转到指定节点
// ============================================================
void StoryEngine::jumpToNode(const QString& nodeId) {
    // 结束标记
    if (nodeId == "END" || nodeId.isEmpty()) {
        qDebug() << "[StoryEngine] Script finished";
        currentNodeId_.clear();
        emit scriptFinished();
        return;
    }

    QJsonObject node = findNode(scriptRoot_, nodeId);
    if (node.isEmpty()) {
        qWarning() << "[StoryEngine] Node not found:" << nodeId;
        emit scriptFinished();
        return;
    }

    // 检查 condition（节点级条件，不满足则跳过）
    QString cond = node.value("condition").toString();
    if (!cond.isEmpty() && !evaluateCondition(cond)) {
        qDebug() << "[StoryEngine] Node" << nodeId << "condition failed, skipping";
        // 简单处理：直接结束剧本（实际项目里可改为跳到下一个节点）
        emit scriptFinished();
        return;
    }

    // 应用节点级 effects（即"进入这个节点时"自动触发的效果）
    applyEffects(node);

    currentNodeId_ = nodeId;
    qDebug() << "[StoryEngine] Jump to node:" << nodeId
             << " speaker:" << node.value("speaker").toString();
    emit nodeChanged();
}

// ============================================================
// 玩家选择了某个选项
// ============================================================
void StoryEngine::onChoiceSelected(int choiceIndex) {
    QJsonObject node = findNode(scriptRoot_, currentNodeId_);
    QJsonArray choices = node.value("choices").toArray();

    if (choiceIndex < 0 || choiceIndex >= choices.size()) {
        qWarning() << "[StoryEngine] Invalid choice index:" << choiceIndex
                   << " in node:" << currentNodeId_;
        return;
    }

    QJsonObject choice = choices[choiceIndex].toObject();
    qDebug() << "[StoryEngine] Choice selected:" << choiceIndex
             << " text:" << choice.value("text").toString();

    // 应用选项的 effects
    applyEffects(choice);

    // 跳转到 next 节点
    QString next = choice.value("next").toString();
    jumpToNode(next);
}

// ============================================================
// 取当前节点信息（供 UI 显示）
// ============================================================
QString StoryEngine::currentSpeaker() const {
    QJsonObject node = findNode(scriptRoot_, currentNodeId_);
    QString speakerKey = node.value("speaker").toString();

    // 把内部 key 转成显示名（如 "ProgDesign" → "程设"）
    if (speakerKey == "narrator") return QString();  // 旁白不显示名字
    if (speakerKey == "player")   return QStringLiteral("我");
    if (speakerKey == "ProgDesign")     return subjectName(SubjectType::ProgDesign);
    if (speakerKey == "Calculus")        return subjectName(SubjectType::Calculus);
    if (speakerKey == "LinearAlgebra")   return subjectName(SubjectType::LinearAlgebra);
    if (speakerKey == "AIIntro")         return subjectName(SubjectType::AIIntro);
    return speakerKey;  // 兜底
}

QString StoryEngine::currentText() const {
    QJsonObject node = findNode(scriptRoot_, currentNodeId_);
    return node.value("text").toString();
}

QStringList StoryEngine::currentChoices() const {
    QJsonObject node = findNode(scriptRoot_, currentNodeId_);
    QJsonArray choices = node.value("choices").toArray();

    QStringList result;
    for (const auto& v : choices) {
        QJsonObject c = v.toObject();
        // 检查选项级 condition（不满足则不显示该选项）
        QString cond = c.value("condition").toString();
        if (!cond.isEmpty() && !evaluateCondition(cond)) {
            continue;
        }
        result.append(c.value("text").toString());
    }
    return result;
}

// ============================================================
// 应用 effects 数组：修改玩家数值
// ============================================================
void StoryEngine::applyEffects(const QJsonObject& obj) {
    QJsonArray effects = obj.value("effects").toArray();
    if (effects.isEmpty()) return;
    if (!player_) {
        qWarning() << "[StoryEngine] applyEffects: player is null";
        return;
    }

    for (const auto& v : effects) {
        QJsonObject eff = v.toObject();
        QString target = eff.value("target").toString();
        int delta = eff.value("delta").toInt();

        if (target.startsWith("affinity.")) {
            QString subjectName = target.mid(QString("affinity.").length());
            SubjectType s;
            if      (subjectName == "ProgDesign")    s = SubjectType::ProgDesign;
            else if (subjectName == "Calculus")      s = SubjectType::Calculus;
            else if (subjectName == "LinearAlgebra") s = SubjectType::LinearAlgebra;
            else if (subjectName == "AIIntro")       s = SubjectType::AIIntro;
            else {
                qWarning() << "[StoryEngine] Unknown subject:" << subjectName;
                continue;
            }
            player_->addAffinity(s, delta);
            qDebug() << "  [effect] affinity." << subjectName << "+=" << delta;
        }
        else if (target == "stress") {
            player_->addStress(delta);
            qDebug() << "  [effect] stress +=" << delta;
        }
        else if (target == "darkness") {
            player_->addDarkness(delta);
            qDebug() << "  [effect] darkness +=" << delta;
        }
        else {
            qWarning() << "[StoryEngine] Unknown effect target:" << target;
        }
    }

    // 发出 effects 触发信号（UI 可监听后做闪烁动画提示）
    emit effectsApplied();
}

// ============================================================
// 评估条件表达式
//
// 支持的语法（简单实现，够用）：
//   <左变量> <运算符> <右数字>
//   <左表达式> && <右表达式>     -- 且
//   <左表达式> || <右表达式>     -- 或
//
// 变量：
//   affinity.ProgDesign / affinity.Calculus / affinity.LinearAlgebra / affinity.AIIntro
//   stress
//   darkness
//   week
//   day
//
// 运算符：> >= < <= == !=
//
// 例：
//   "affinity.LinearAlgebra >= 50"
//   "stress < 80 && darkness < 50"
//   "week >= 3 || affinity.ProgDesign > 30"
// ============================================================
int StoryEngine::readVariable(const QString& name) const {
    if (!player_) return 0;

    if (name == "stress")   return player_->stress();
    if (name == "darkness") return player_->darkness();
    if (name == "week")     return player_->currentWeek();
    if (name == "day")      return player_->currentDay();

    if (name.startsWith("affinity.")) {
        QString s = name.mid(QString("affinity.").length());
        if (s == "ProgDesign")    return player_->affinity(SubjectType::ProgDesign);
        if (s == "Calculus")      return player_->affinity(SubjectType::Calculus);
        if (s == "LinearAlgebra") return player_->affinity(SubjectType::LinearAlgebra);
        if (s == "AIIntro")       return player_->affinity(SubjectType::AIIntro);
    }

    qWarning() << "[StoryEngine] Unknown variable in condition:" << name;
    return 0;
}

bool StoryEngine::evaluateAtomic(const QString& expr) const {
    // 解析形如 "affinity.X >= 50" 的单条原子表达式
    // 用正则匹配：变量名 运算符 数字
    static const QRegularExpression re(
        R"(^\s*([a-zA-Z][\w.]*)\s*(>=|<=|==|!=|>|<)\s*(-?\d+)\s*$)"
    );
    QRegularExpressionMatch m = re.match(expr);
    if (!m.hasMatch()) {
        qWarning() << "[StoryEngine] Invalid atomic condition:" << expr;
        return true;  // 出错时默认放行（避免剧情卡死）
    }

    QString varName = m.captured(1);
    QString op = m.captured(2);
    int rhs = m.captured(3).toInt();
    int lhs = readVariable(varName);

    if (op == ">")  return lhs >  rhs;
    if (op == ">=") return lhs >= rhs;
    if (op == "<")  return lhs <  rhs;
    if (op == "<=") return lhs <= rhs;
    if (op == "==") return lhs == rhs;
    if (op == "!=") return lhs != rhs;
    return true;
}

bool StoryEngine::evaluateCondition(const QString& expression) const {
    if (expression.trimmed().isEmpty()) return true;

    // 处理 || 优先级最低，先按 || 拆分
    if (expression.contains("||")) {
        QStringList parts = expression.split("||");
        for (const QString& p : parts) {
            if (evaluateCondition(p)) return true;  // 任一为真则真
        }
        return false;
    }
    // 然后按 && 拆分
    if (expression.contains("&&")) {
        QStringList parts = expression.split("&&");
        for (const QString& p : parts) {
            if (!evaluateCondition(p)) return false;  // 任一为假则假
        }
        return true;
    }
    // 没有逻辑运算符 = 单条原子表达式
    return evaluateAtomic(expression);
}

} // namespace SA
