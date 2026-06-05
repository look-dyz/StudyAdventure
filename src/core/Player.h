#ifndef STUDYADVENTURE_PLAYER_H
#define STUDYADVENTURE_PLAYER_H
#include <QObject>
#include <QMap>
#include <QSet>
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

            // ========== 黑化值 ==========
    int darkness() const { return darkness_; }
    void setDarkness(int value);
    void addDarkness(int delta);

            // ========== 日期 ==========
    int currentWeek() const { return currentWeek_; }
    int currentDay()  const { return currentDay_;  }
    void advanceDay();
    void setCurrentWeek(int week);
    void setCurrentDay(int day);

            // ========== 剧本完成记录 ==========
            /// 标记某个剧本路径已完成
    void markScriptFinished(const QString& scriptPath);
    /// 查询某个剧本是否已完成
    bool isScriptFinished(const QString& scriptPath) const;

            // ========== 自由活动天数 ==========
            /// 本周剩余自由活动天数（每周7天，上课/剧情消耗）
    int freeDaysLeft() const { return freeDaysLeft_; }
    void setFreeDaysLeft(int days);
    void consumeFreeDay();   ///< 消耗一天自由活动

            // ========== 重置 ==========
    void reset();

signals:
    void affinityChanged(SubjectType s, int newValue);
    void stressChanged(int newValue);
    void darknessChanged(int newValue);
    void dateChanged(int week, int day);
    void freeDaysChanged(int daysLeft);
    void weekAdvanced(int newWeek);

private:
    QMap<SubjectType, int> affinityMap_;
    int stress_       = 0;
    int darkness_     = 0;
    int currentWeek_  = 1;
    int currentDay_   = 1;
    int freeDaysLeft_ = 6;   // 每周开始时重置（第1天固定触发剧情）
    QSet<QString> finishedScripts_;  // 已完成的剧本路径集合
};
} // namespace SA
#endif