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
    void markScriptFinished(const QString& scriptPath);
    bool isScriptFinished(const QString& scriptPath) const;
    QSet<QString> finishedScripts() const { return finishedScripts_; }

            // ========== 自由活动天数 ==========
    int freeDaysLeft() const { return freeDaysLeft_; }
    void setFreeDaysLeft(int days);
    void consumeFreeDay();

            // ========== 存档剧情位置 ==========
    QString savedScriptPath() const { return savedScriptPath_; }
    QString savedNodeId()     const { return savedNodeId_; }
    void setSavedScriptPath(const QString& p) { savedScriptPath_ = p; }
    void setSavedNodeId(const QString& id)    { savedNodeId_ = id; }

            // ========== 重置 ==========
    void reset();

public:
    bool choseToStay() const { return choseToStay_; }
    void setChoseToStay(bool v) { choseToStay_ = v; }

private:
    bool choseToStay_ = false;

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
    int freeDaysLeft_ = 7;
    QSet<QString> finishedScripts_;
    QString savedScriptPath_;
    QString savedNodeId_;
};

} // namespace SA
#endif // STUDYADVENTURE_PLAYER_H