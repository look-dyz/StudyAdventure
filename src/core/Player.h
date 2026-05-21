#pragma once

#include <QObject>
#include <QString>
#include <map>

#include "common/Constants.h"

namespace SA {

class Player : public QObject
{
    Q_OBJECT

public:
    explicit Player(QObject* parent = nullptr);

            // =========================
            // 基础属性
            // =========================
    int getHealth() const;
    int getEnergy() const;
    int getKnowledge() const;

    QString getName() const;

    void setName(const QString& name);

            // =========================
            // 属性修改
            // =========================
    void addHealth(int value);
    void addEnergy(int value);
    void addKnowledge(int value);

    void addStress(int value);
    void addDarkness(int value);

    void addAffinity(SubjectType subject, int value);

            // =========================
            // 时间系统
            // =========================
    void advanceDay();

    int currentWeek() const;
    int currentDay() const;

            // =========================
            // 查询
            // =========================
    int stress() const;
    int darkness() const;

    int affinity(SubjectType subject) const;

signals:
    void healthChanged(int);
    void energyChanged(int);
    void knowledgeChanged(int);

    void stressChanged(int);
    void darknessChanged(int);

    void affinityChanged(SubjectType, int);

    void dateChanged(int week, int day);

private:
    int m_health;
    int m_energy;
    int m_knowledge;

    int m_stress;
    int m_darkness;

    int m_week;
    int m_day;

    QString m_name;

    std::map<SubjectType, int> m_affinity;
};

} // namespace SA