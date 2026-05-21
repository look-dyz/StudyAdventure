#include "Player.h"

#include <algorithm>

namespace SA {

Player::Player(QObject* parent)
    : QObject(parent),
      m_health(100),
      m_energy(100),
      m_knowledge(0),
      m_stress(0),
      m_darkness(0),
      m_week(1),
      m_day(1),
      m_name("Player")
{
}

int Player::getHealth() const
{
    return m_health;
}

int Player::getEnergy() const
{
    return m_energy;
}

int Player::getKnowledge() const
{
    return m_knowledge;
}

QString Player::getName() const
{
    return m_name;
}

void Player::setName(const QString& name)
{
    m_name = name;
}

void Player::addHealth(int value)
{
    m_health += value;

    m_health = std::clamp(m_health, 0, 100);

    emit healthChanged(m_health);
}

void Player::addEnergy(int value)
{
    m_energy += value;

    m_energy = std::clamp(m_energy, 0, 100);

    emit energyChanged(m_energy);
}

void Player::addKnowledge(int value)
{
    m_knowledge += value;

    m_knowledge = std::max(0, m_knowledge);

    emit knowledgeChanged(m_knowledge);
}

void Player::addStress(int value)
{
    m_stress += value;

    m_stress = std::clamp(m_stress, 0, 100);

    emit stressChanged(m_stress);
}

void Player::addDarkness(int value)
{
    m_darkness += value;

    m_darkness = std::clamp(m_darkness, 0, 100);

    emit darknessChanged(m_darkness);
}

void Player::addAffinity(SubjectType subject, int value)
{
    m_affinity[subject] += value;

    m_affinity[subject] =
        std::clamp(m_affinity[subject], 0, 100);

    emit affinityChanged(subject, m_affinity[subject]);
}

void Player::advanceDay()
{
    m_day++;

    if (m_day > 7) {
        m_day = 1;
        m_week++;
    }

    emit dateChanged(m_week, m_day);
}

int Player::currentWeek() const
{
    return m_week;
}

int Player::currentDay() const
{
    return m_day;
}

int Player::stress() const
{
    return m_stress;
}

int Player::darkness() const
{
    return m_darkness;
}

int Player::affinity(SubjectType subject) const
{
    auto it = m_affinity.find(subject);

    if (it != m_affinity.end()) {
        return it->second;
    }

    return 0;
}

} // namespace SA