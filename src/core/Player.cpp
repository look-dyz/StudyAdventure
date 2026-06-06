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
void Player::addStress(int delta) { setStress(stress_ + delta); }

// ========== 黑化值 ==========
void Player::setDarkness(int value) {
    int clamped = std::clamp(value, DARKNESS_MIN, DARKNESS_MAX);
    if (darkness_ != clamped) {
        darkness_ = clamped;
        emit darknessChanged(darkness_);
    }
}
void Player::addDarkness(int delta) { setDarkness(darkness_ + delta); }

// ========== 日期 ==========
void Player::advanceDay() {
    currentDay_++;
    if (currentDay_ > 7) {
        currentDay_ = 1;
        currentWeek_++;
        freeDaysLeft_ = 6;
        emit freeDaysChanged(freeDaysLeft_);
        emit weekAdvanced(currentWeek_);   // ← 新增
    }
    emit dateChanged(currentWeek_, currentDay_);
}
void Player::setCurrentWeek(int week) { currentWeek_ = qMax(1, week);
    emit dateChanged(currentWeek_, currentDay_);  }
void Player::setCurrentDay(int day)   { currentDay_ = qBound(1, day, 7); }

// ========== 剧本完成记录 ==========
void Player::markScriptFinished(const QString& scriptPath) {
    finishedScripts_.insert(scriptPath);
}
bool Player::isScriptFinished(const QString& scriptPath) const {
    return finishedScripts_.contains(scriptPath);
}

// ========== 自由活动天数 ==========
void Player::setFreeDaysLeft(int days) {
    int clamped = qMax(0, days);
    if (freeDaysLeft_ != clamped) {
        freeDaysLeft_ = clamped;
        emit freeDaysChanged(freeDaysLeft_);
    }
}
void Player::consumeFreeDay() {
    int newVal = qMax(0, freeDaysLeft_ - 1);
    if (freeDaysLeft_ != newVal) {
        freeDaysLeft_ = newVal;
        emit freeDaysChanged(freeDaysLeft_);
    }
    // 自由天数用完时，自动推进到下一周（触发 weekAdvanced）
    if (freeDaysLeft_ == 0) {
        currentDay_ = 7;
        advanceDay();
    }
}

// ========== 重置 ==========
void Player::reset() {
    affinityMap_[SubjectType::ProgDesign]    = 0;
    affinityMap_[SubjectType::Calculus]      = 0;
    affinityMap_[SubjectType::LinearAlgebra] = 0;
    affinityMap_[SubjectType::AIIntro]       = 0;
    stress_       = 0;
    darkness_     = 0;
    currentWeek_  = 1;
    currentDay_   = 1;
    freeDaysLeft_ = 6;
    finishedScripts_.clear();
}

} // namespace SA