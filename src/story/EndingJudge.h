/**
 * @file EndingJudge.h
 * @brief 结局判定器
 *
 * 根据玩家当前数值判定应触发哪个结局。
 * 优先级链：压力满 > 黑化满 > 好感+留下 > 好感+离开 > 默认
 *
 * 负责人：成员 A
 */
#ifndef STUDYADVENTURE_ENDINGJUDGE_H
#define STUDYADVENTURE_ENDINGJUDGE_H

#include "common/Constants.h"

namespace SA {

class Player;

class EndingJudge {
public:
    /// 判定当前应触发的结局
    /// @param player 玩家数据
    /// @param choseToStay 玩家在期末是否选择"留下"（影响 End2 vs End3）
    /// @return 结局类型
    static EndingType judge(const Player* player, bool choseToStay);
};

} // namespace SA

#endif // STUDYADVENTURE_ENDINGJUDGE_H
