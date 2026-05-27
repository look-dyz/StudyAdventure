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
    if(!player) {
        qDebug() << "[SaveManager] save failed: player is null";
        return false;
    }

    QJsonObject root;

            // =========================
            // 基础数值
            // =========================

    root["stress"] = player->stress();
    root["darkness"] = player->darkness();
    root["week"] = player->currentWeek();
    root["day"] = player->currentDay();

            // =========================
            // 好感度
            // =========================

    QJsonObject affinityObj;

    affinityObj["ProgDesign"] =
        player->affinity(SubjectType::ProgDesign);

    affinityObj["Calculus"] =
        player->affinity(SubjectType::Calculus);

    affinityObj["LinearAlgebra"] =
        player->affinity(SubjectType::LinearAlgebra);

    affinityObj["AIIntro"] =
        player->affinity(SubjectType::AIIntro);

    root["affinity"] = affinityObj;

            // =========================
            // JSON 文档
            // =========================

    QJsonDocument doc(root);

    QFile file(savePath(slot));

    if(!file.open(QIODevice::WriteOnly)) {

        qDebug() << "[SaveManager] Failed to open file:"
                 << file.fileName();

        return false;
    }

    file.write(doc.toJson());

    file.close();

    qDebug() << "[SaveManager] Saved to"
             << file.fileName();

    return true;
}

bool SaveManager::load(Player* player, int slot)
{
    if(!player) {

        qDebug() << "[SaveManager] load failed: player is null";

        return false;
    }

    QFile file(savePath(slot));

    if(!file.exists()) {

        qDebug() << "[SaveManager] Save file does not exist";

        return false;
    }

    if(!file.open(QIODevice::ReadOnly)) {

        qDebug() << "[SaveManager] Failed to open save file";

        return false;
    }

    QByteArray data = file.readAll();

    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);

    if(doc.isNull()) {

        qDebug() << "[SaveManager] Invalid JSON";

        return false;
    }

    QJsonObject root = doc.object();

            // =========================
            // 基础数值
            // =========================

    player->setStress(
        root["stress"].toInt()
        );

    player->setDarkness(
        root["darkness"].toInt()
        );

    player->setCurrentWeek(
        root["week"].toInt()
        );

    player->setCurrentDay(
        root["day"].toInt()
        );

            // =========================
            // 好感度
            // =========================

    QJsonObject affinityObj =
        root["affinity"].toObject();

    player->setAffinity(
        SubjectType::ProgDesign,
        affinityObj["ProgDesign"].toInt()
        );

    player->setAffinity(
        SubjectType::Calculus,
        affinityObj["Calculus"].toInt()
        );

    player->setAffinity(
        SubjectType::LinearAlgebra,
        affinityObj["LinearAlgebra"].toInt()
        );

    player->setAffinity(
        SubjectType::AIIntro,
        affinityObj["AIIntro"].toInt()
        );

    qDebug() << "[SaveManager] Loaded from"
             << file.fileName();

    return true;
}

bool SaveManager::exists(int slot) {
    return QFile::exists(savePath(slot));
}

bool SaveManager::remove(int slot) {
    return QFile::remove(savePath(slot));
}

} // namespace SA
