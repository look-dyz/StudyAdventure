/**
 * @file SaveManager.h
 * @brief 存档读档管理
 *
 * 用 QJsonDocument 序列化玩家数据为 JSON 文件存储。
 * 支持多存档槽（save_1.json ~ save_3.json）。
 *
 * 负责人：成员 C
 */
#ifndef STUDYADVENTURE_SAVEMANAGER_H
#define STUDYADVENTURE_SAVEMANAGER_H

#include <QString>

namespace SA {

class Player;

class SaveManager {
public:
    /// 保存到指定存档槽
    /// @param player 要保存的玩家数据
    /// @param slot 存档槽编号（1-3）
    /// @return 成功返回 true
    static bool save(const Player* player, int slot);

    /// 从指定存档槽读取
    /// @param player 要写入的玩家对象
    /// @param slot 存档槽编号（1-3）
    /// @return 成功返回 true
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
