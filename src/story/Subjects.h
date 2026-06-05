/**
 * @file Subjects.h
 * @brief 四个学科角色派生类
 *
 * 派生自 common/Subject.h 的基类，每个角色实现自己的 interact() 和 createGame()。
 * 负责人：成员 A 维护
 */
#ifndef STUDYADVENTURE_SUBJECTS_H
#define STUDYADVENTURE_SUBJECTS_H

#include "common/Subject.h"

namespace SA {

// ========== 程设 ==========
class ProgDesignSubject : public Subject {
    Q_OBJECT
public:
    explicit ProgDesignSubject(QObject* parent = nullptr)
        : Subject(SubjectType::ProgDesign, parent) {}

    void interact() override;
    MiniGame* createGame() override;   ///< 创建扫雷或 21 点（随机/玩家选）
};

// ========== 高数 ==========
class CalculusSubject : public Subject {
    Q_OBJECT
public:
    explicit CalculusSubject(QObject* parent = nullptr)
        : Subject(SubjectType::Calculus, parent) {}

    void interact() override;
    MiniGame* createGame() override;   ///< 高数答题
};

// ========== 线代 ==========
class LinearAlgebraSubject : public Subject {
    Q_OBJECT
public:
    explicit LinearAlgebraSubject(QObject* parent = nullptr)
        : Subject(SubjectType::LinearAlgebra, parent) {}

    void interact() override;
    MiniGame* createGame() override;   ///< 矩阵运算
};

// ========== AI 引论 ==========
class AIIntroSubject : public Subject {
    Q_OBJECT
public:
    explicit AIIntroSubject(QObject* parent = nullptr)
        : Subject(SubjectType::AIIntro, parent) {}

    void interact() override;
    MiniGame* createGame() override;   ///< 井字棋或 AI 迷宫
};

} // namespace SA

#endif // STUDYADVENTURE_SUBJECTS_H
