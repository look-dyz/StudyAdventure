#include "Player.h"
#include <algorithm>

namespace SA {

Player::Player(QObject* parent) : QObject(parent) {
    reset();
}

// ========== 好感度 ==========

int Player::affinity(SubjectType s) const {
    return affinityMap_.value(s, 0);
}

void Player::setAffinity(SubjectType s, int value) {
    int clamped = std::clamp(value, AFFINITY_MIN, AFFINITY_MAX);
    if (affinityMap_.value(s, -1) != clamped) {
        affinityMap_[s] = clamped;
        emit affinityChanged(s, clamped);
    }
}

void Player::addAffinity(SubjectType s, int delta) {
    setAffinity(s, affinity(s) + delta);
}

// ========== 压力值 ==========

void Player::setStress(int value) {
    int clamped = std::clamp(value, STRESS_MIN, STRESS_MAX);
    if (stress_ != clamped) {
        stress_ = clamped;
        emit stressChanged(stress_);
    }
}

void Player::addStress(int delta) {
    setStress(stress_ + delta);
}

// ========== 黑化值 ==========

void Player::setDarkness(int value) {
    int clamped = std::clamp(value, DARKNESS_MIN, DARKNESS_MAX);
    if (darkness_ != clamped) {
        darkness_ = clamped;
        emit darknessChanged(darkness_);
    }
}

void Player::addDarkness(int delta) {
    setDarkness(darkness_ + delta);
}

// ========== 日期 ==========

void Player::advanceDay() {
    currentDay_++;
    if (currentDay_ > 7) {
        currentDay_ = 1;
        currentWeek_++;
    }
    emit dateChanged(currentWeek_, currentDay_);
}

// ========== 重置 ==========

void Player::reset() {
    affinityMap_[SubjectType::ProgDesign]    = 0;
    affinityMap_[SubjectType::Calculus]      = 0;
    affinityMap_[SubjectType::LinearAlgebra] = 0;
    affinityMap_[SubjectType::AIIntro]       = 0;
    stress_ = 0;
    darkness_ = 0;
    currentWeek_ = 1;
    currentDay_ = 1;
}

} // namespace SA
