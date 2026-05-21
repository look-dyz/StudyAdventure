#include "SaveManager.h"

#include "GameManager.h"
#include "Player.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

namespace SA {

bool SaveManager::saveGame(const QString& path)
{
    QFile file(path);

    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    Player& player = GameManager::instance().getPlayer();

    QJsonObject obj;

    obj["name"] = player.getName();

    obj["health"] = player.getHealth();
    obj["energy"] = player.getEnergy();
    obj["knowledge"] = player.getKnowledge();

    obj["stress"] = player.stress();
    obj["darkness"] = player.darkness();

    obj["week"] = player.currentWeek();
    obj["day"] = player.currentDay();

    obj["progdesign_affinity"] =
        player.affinity(SubjectType::ProgDesign);

    obj["calculus_affinity"] =
        player.affinity(SubjectType::Calculus);

    obj["linearalgebra_affinity"] =
        player.affinity(SubjectType::LinearAlgebra);

    obj["aiintro_affinity"] =
        player.affinity(SubjectType::AIIntro);

    QJsonDocument doc(obj);

    file.write(doc.toJson());

    file.close();

    return true;
}

bool SaveManager::loadGame(const QString& path)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray data = file.readAll();

    file.close();

    QJsonDocument doc =
        QJsonDocument::fromJson(data);

    if (!doc.isObject()) {
        return false;
    }

    QJsonObject obj = doc.object();

    Player& player =
        GameManager::instance().getPlayer();

    player.setName(obj["name"].toString());

    player.addHealth(
        obj["health"].toInt() - player.getHealth());

    player.addEnergy(
        obj["energy"].toInt() - player.getEnergy());

    player.addKnowledge(
        obj["knowledge"].toInt() - player.getKnowledge());

    player.addStress(
        obj["stress"].toInt() - player.stress());

    player.addDarkness(
        obj["darkness"].toInt() - player.darkness());

    player.addAffinity(
        SubjectType::ProgDesign,
        obj["progdesign_affinity"].toInt()
            - player.affinity(SubjectType::ProgDesign));

    player.addAffinity(
        SubjectType::Calculus,
        obj["calculus_affinity"].toInt()
            - player.affinity(SubjectType::Calculus));

    player.addAffinity(
        SubjectType::LinearAlgebra,
        obj["linearalgebra_affinity"].toInt()
            - player.affinity(SubjectType::LinearAlgebra));

    player.addAffinity(
        SubjectType::AIIntro,
        obj["aiintro_affinity"].toInt()
            - player.affinity(SubjectType::AIIntro));

    return true;
}

} // namespace SA