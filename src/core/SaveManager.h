#ifndef STUDYADVENTURE_SAVEMANAGER_H
#define STUDYADVENTURE_SAVEMANAGER_H

#include <QString>

namespace SA {

class Player;

class SaveManager {
public:
    /// 保存到指定存档槽
    static bool save(const Player* player, int slot);
    /// 从指定存档槽读取
    static bool load(Player* player, int slot);
    /// 检查存档槽是否存在
    static bool exists(int slot);
    /// 删除指定存档
    static bool remove(int slot);

private:
    static QString savePath(int slot);
};

} // namespace SA
#endif // STUDYADVENTURE_SAVEMANAGER_H