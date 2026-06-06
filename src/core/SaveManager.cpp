#include "SaveManager.h"
#include "Player.h"
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace SA {

QString SaveManager::savePath(int slot) {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return QString("%1/save_%2.json").arg(dir).arg(slot);
}

bool SaveManager::save(const Player* player, int slot)
{
    if (!player) {
        qDebug() << "[SaveManager] save failed: player is null";
        return false;
    }

    QJsonObject root;

            // =========================
            // 基础数值
            // =========================
    root["stress"]      = player->stress();
    root["darkness"]    = player->darkness();
    root["week"]        = player->currentWeek();
    root["day"]         = player->currentDay();
    root["freeDaysLeft"] = player->freeDaysLeft();

            // =========================
            // 好感度
            // =========================
    QJsonObject affinityObj;
    affinityObj["ProgDesign"]    = player->affinity(SubjectType::ProgDesign);
    affinityObj["Calculus"]      = player->affinity(SubjectType::Calculus);
    affinityObj["LinearAlgebra"] = player->affinity(SubjectType::LinearAlgebra);
    affinityObj["AIIntro"]       = player->affinity(SubjectType::AIIntro);
    root["affinity"] = affinityObj;

            // =========================
            // 已完成剧本
            // =========================
    QJsonArray scriptsArr;
    for (const QString& path : player->finishedScripts())
        scriptsArr.append(path);
    root["finishedScripts"] = scriptsArr;

            // =========================
            // 剧情存档点
            // =========================
    root["savedScriptPath"] = player->savedScriptPath();
    root["savedNodeId"]     = player->savedNodeId();

            // =========================
            // 写入文件
            // =========================
    QJsonDocument doc(root);
    QFile file(savePath(slot));
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "[SaveManager] Failed to open file:" << file.fileName();
        return false;
    }
    file.write(doc.toJson());
    file.close();
    qDebug() << "[SaveManager] Saved to" << file.fileName();
    return true;
}

bool SaveManager::load(Player* player, int slot)
{
    if (!player) {
        qDebug() << "[SaveManager] load failed: player is null";
        return false;
    }

    QFile file(savePath(slot));
    if (!file.exists()) {
        qDebug() << "[SaveManager] Save file does not exist";
        return false;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "[SaveManager] Failed to open save file";
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qDebug() << "[SaveManager] Invalid JSON";
        return false;
    }

    QJsonObject root = doc.object();

            // =========================
            // 基础数值
            // =========================
    player->setStress(root["stress"].toInt());
    player->setDarkness(root["darkness"].toInt());
    player->setCurrentWeek(root["week"].toInt());
    player->setCurrentDay(root["day"].toInt());
    player->setFreeDaysLeft(root["freeDaysLeft"].toInt(7));

            // =========================
            // 好感度
            // =========================
    QJsonObject affinityObj = root["affinity"].toObject();
    player->setAffinity(SubjectType::ProgDesign,    affinityObj["ProgDesign"].toInt());
    player->setAffinity(SubjectType::Calculus,      affinityObj["Calculus"].toInt());
    player->setAffinity(SubjectType::LinearAlgebra, affinityObj["LinearAlgebra"].toInt());
    player->setAffinity(SubjectType::AIIntro,       affinityObj["AIIntro"].toInt());

            // =========================
            // 已完成剧本
            // =========================
    QJsonArray scriptsArr = root["finishedScripts"].toArray();
    for (const auto& v : scriptsArr)
        player->markScriptFinished(v.toString());

            // =========================
            // 剧情存档点
            // =========================
    player->setSavedScriptPath(root["savedScriptPath"].toString());
    player->setSavedNodeId(root["savedNodeId"].toString());

    qDebug() << "[SaveManager] Loaded from" << file.fileName();
    return true;
}

bool SaveManager::exists(int slot) {
    return QFile::exists(savePath(slot));
}

bool SaveManager::remove(int slot) {
    return QFile::remove(savePath(slot));
}

} // namespace SA