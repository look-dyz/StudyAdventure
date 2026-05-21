#include "SaveManager.h"
#include "Player.h"
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

namespace SA {

QString SaveManager::savePath(int slot) {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return QString("%1/save_%2.json").arg(dir).arg(slot);
}

bool SaveManager::save(const Player* player, int slot) {
    // TODO（成员 C）：序列化 Player 数据为 JSON 并写入文件
    Q_UNUSED(player);
    qDebug() << "[SaveManager] save() to slot" << slot << "(not implemented yet)";
    return false;
}

bool SaveManager::load(Player* player, int slot) {
    // TODO（成员 C）：从 JSON 文件读取并填充 Player 数据
    Q_UNUSED(player);
    qDebug() << "[SaveManager] load() from slot" << slot << "(not implemented yet)";
    return false;
}

bool SaveManager::exists(int slot) {
    return QFile::exists(savePath(slot));
}

bool SaveManager::remove(int slot) {
    return QFile::remove(savePath(slot));
}

} // namespace SA
