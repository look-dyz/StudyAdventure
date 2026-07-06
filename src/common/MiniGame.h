/**
 * @file MiniGame.h
 * @brief 所有小游戏的抽象基类
 *
 * 5 个小游戏（扫雷/21点/井字棋/记忆翻牌/AI迷宫）
 * 全部派生自此基类。负责人：成员 C
 *
 * ===== 接口约定 =====
 * 1. 派生类必须实现 start() 和 reset() 纯虚函数
 * 2. 游戏结束时必须发出 finished(score, won) 信号
 * 3. 不直接修改玩家数值，由 GameManager 监听信号统一处理
 */
#ifndef STUDYADVENTURE_MINIGAME_H
#define STUDYADVENTURE_MINIGAME_H

#include <QWidget>
#include "Constants.h"

namespace SA {

class MiniGame : public QWidget {
    Q_OBJECT
public:
    explicit MiniGame(QWidget* parent = nullptr) : QWidget(parent) {}
    virtual ~MiniGame() = default;

    /// 开始游戏，初始化棋盘等
    virtual void start() = 0;

    /// 重置游戏到初始状态
    virtual void reset() = 0;

    /// 返回小游戏类型
    virtual MiniGameType type() const = 0;

    /// 返回游戏显示名称
    virtual QString displayName() const = 0;

    /// 设置难度（影响内部参数）
    void setDifficulty(Difficulty d) { difficulty_ = d; }
    Difficulty difficulty() const { return difficulty_; }

signals:
    /// 游戏结束信号
    /// @param score 得分（0-100）
    /// @param won 是否胜利
    void finished(int score, bool won);

    /// 中途产生压力（如踩雷、答错等）
    /// @param delta 压力增量
    void stressOccurred(int delta);

protected:
    Difficulty difficulty_ = Difficulty::Easy;
};

} // namespace SA

#endif // STUDYADVENTURE_MINIGAME_H
