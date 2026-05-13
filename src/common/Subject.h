/**
 * @file Subject.h
 * @brief 所有学科角色的抽象基类
 *
 * 4 个学科角色（程设/高数/线代/AI引论）全部派生自此基类。
 * 负责人：成员 C 设计基类，成员 A 使用并实现派生类的特定行为
 *
 * ===== 接口约定 =====
 * 1. 派生类必须实现 interact() 和 createGame() 纯虚函数
 * 2. createGame() 返回的 MiniGame* 由调用方接管所有权（建议用 unique_ptr 持有）
 */
#ifndef STUDYADVENTURE_SUBJECT_H
#define STUDYADVENTURE_SUBJECT_H

#include <QString>
#include <QObject>
#include "Constants.h"
#include "MiniGame.h"

namespace SA {

class Subject : public QObject {
    Q_OBJECT
public:
    explicit Subject(SubjectType type, QObject* parent = nullptr)
        : QObject(parent), type_(type) {}
    virtual ~Subject() = default;

    /// 学科类型
    SubjectType type() const { return type_; }

    /// 学科显示名称
    QString name() const { return subjectName(type_); }

    /// 当前好感度
    int affinity() const { return affinity_; }

    /// 修改好感度（夹紧到 [0, 100]）
    void changeAffinity(int delta);

    /// 与该学科互动（触发对话/事件），派生类实现
    virtual void interact() = 0;

    /// 创建该学科专属的小游戏
    /// 返回新创建的对象，所有权转移给调用方
    virtual MiniGame* createGame() = 0;

signals:
    /// 好感度变化信号（由 UI 监听更新进度条）
    void affinityChanged(int newValue);

protected:
    SubjectType type_;
    int affinity_ = 0;
};

// ============================================================
// 四个派生类的前向声明（由成员 A 在 src/story/ 下实现）
// ============================================================
class ProgDesignSubject;       ///< 程设
class CalculusSubject;          ///< 高数
class LinearAlgebraSubject;     ///< 线代
class AIIntroSubject;           ///< AI引论

} // namespace SA

#endif // STUDYADVENTURE_SUBJECT_H
