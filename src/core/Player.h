/**
 * @file Player.h
 * @brief 玩家数值系统
 *
 * 管理玩家的所有数值：4 个学科好感度、压力值、线代黑化值、当前日期等。
 * 负责人：成员 C
 *
 * ===== 接口约定 =====
 * 1. 所有数值修改必须通过 setter，会自动夹紧到合法区间
 * 2. setter 发出对应信号驱动 UI 刷新（信号-槽解耦）
 * 3. 不直接被 UI 持有；UI 监听信号即可
 */
#ifndef STUDYADVENTURE_PLAYER_H
#define STUDYADVENTURE_PLAYER_H

#include <QObject>
#include <QMap>
#include "common/Constants.h"

namespace SA {

class Player : public QObject {
    Q_OBJECT
public:
    explicit Player(QObject* parent = nullptr);

    // ========== 好感度 ==========
    int affinity(SubjectType s) const;
    void setAffinity(SubjectType s, int value);
    void addAffinity(SubjectType s, int delta);

    // ========== 压力值 ==========
    int stress() const { return stress_; }
    void setStress(int value);
    void addStress(int delta);

    // ========== 黑化值（仅线代）==========
    int darkness() const { return darkness_; }
    void setDarkness(int value);
    void addDarkness(int delta);

    // ========== 游戏内日期/周次 ==========
    int currentWeek() const { return currentWeek_; }
    int currentDay() const { return currentDay_; }
    void advanceDay();          ///< 推进一天
    void setCurrentWeek(int week);
    void setCurrentDay(int day);

    // ========== 重置（新游戏）==========
    void reset();

signals:
    void affinityChanged(SubjectType s, int newValue);
    void stressChanged(int newValue);
    void darknessChanged(int newValue);
    void dateChanged(int week, int day);

private:
    QMap<SubjectType, int> affinityMap_;
    int stress_ = 0;
    int darkness_ = 0;
    int currentWeek_ = 1;
    int currentDay_ = 1;
};

} // namespace SA

#endif // STUDYADVENTURE_PLAYER_H
