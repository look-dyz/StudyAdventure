/**
 * @file Constants.h
 * @brief 全局常量定义
 *
 * 所有跨模块共享的枚举、常量都放在这里。
 * 修改本文件需要在群里通告所有成员。
 */
#ifndef STUDYADVENTURE_CONSTANTS_H
#define STUDYADVENTURE_CONSTANTS_H

#include <QString>

namespace SA {  // Study Adventure 命名空间

// ============================================================
// 学科类型
// ============================================================
enum class SubjectType {
    ProgDesign,      ///< 程序设计
    Calculus,        ///< 高等数学
    LinearAlgebra,   ///< 线性代数
    AIIntro          ///< 人工智能引论
};

// ============================================================
// 小游戏类型
// ============================================================
enum class MiniGameType {
    Minesweeper,     ///< 扫雷（程设）
    Blackjack,       ///< 21点（程设）
    TicTacToe,       ///< 井字棋（AI引论）
    MemoryMatch,
    Maze,            ///< AI迷宫逃生（AI引论）
};

// ============================================================
// 难度等级
// ============================================================
enum class Difficulty {
    Easy,
    Normal,
    Hard
};

// ============================================================
// 游戏场景
// ============================================================
enum class GameScene {
    MainMenu,        ///< 主菜单
    Map,             ///< 地图
    Dialog,          ///< 对话
    MiniGame,        ///< 小游戏
    Ending           ///< 结局
};

// ============================================================
// 地图地点
// ============================================================
enum class Location {
    Classroom,       ///< 教学楼
    Library,         ///< 图书馆
    WeimingLake,     ///< 未名湖
    Dormitory        ///< 宿舍
};

// ============================================================
// 结局类型
// ============================================================
enum class EndingType {
    End1_GoodGrade,      ///< End1「3.92」默认结局
    End2_BestLove,       ///< End2「Best Love」留在异世界
    End3_Farewell,       ///< End3「再见」回到现实
    End4_EternalLA,      ///< End4「永远的线代」线代黑化
    End5_Hospital        ///< End5「陌生的天花板」压力崩溃
};

// ============================================================
// 数值上下限
// ============================================================
constexpr int AFFINITY_MIN = 0;
constexpr int AFFINITY_MAX = 100;
constexpr int STRESS_MIN = 0;
constexpr int STRESS_MAX = 100;
constexpr int DARKNESS_MIN = 0;
constexpr int DARKNESS_MAX = 100;

// 结局触发阈值
constexpr int AFFINITY_THRESHOLD_LOVE = 80;     ///< 触发好感结局的好感度阈值
constexpr int STRESS_THRESHOLD_BREAK = 100;     ///< 压力崩溃阈值（end5）
constexpr int DARKNESS_THRESHOLD_LA = 100;      ///< 线代黑化阈值（end4）

// ============================================================
// 工具函数
// ============================================================
inline QString subjectName(SubjectType s) {
    switch (s) {
        case SubjectType::ProgDesign:    return QStringLiteral("程设");
        case SubjectType::Calculus:      return QStringLiteral("高数");
        case SubjectType::LinearAlgebra: return QStringLiteral("线代");
        case SubjectType::AIIntro:       return QStringLiteral("AI引论");
    }
    return QStringLiteral("未知");
}

inline QString locationName(Location loc) {
    switch (loc) {
        case Location::Classroom:   return QStringLiteral("教学楼");
        case Location::Library:     return QStringLiteral("图书馆");
        case Location::WeimingLake: return QStringLiteral("未名湖");
        case Location::Dormitory:   return QStringLiteral("宿舍");
    }
    return QStringLiteral("未知");
}

} // namespace SA

#endif // STUDYADVENTURE_CONSTANTS_H
