#include "Subjects.h"
#include "games/MinesweeperGame.h"
#include "games/TicTacToeGame.h"
#include "games/BlackjackGame.h"
#include "games/MemoryGame.h"
#include <QDebug>

namespace SA {

// ============================================================
// interact() 设计说明：
//
// 每个学科的 interact() 由 GameManager 调用，触发与该学科的对话。
// 真实剧情通过 StoryEngine 加载对应的 JSON 剧本来呈现，
// 所以这里只是发出信号通知"想跟 X 学科互动"——
// MainWindow 监听后切到对话场景并让 StoryEngine 加载对应剧本。
//
// 这种设计的好处：Subject 类不直接持有 UI 组件，保持解耦。
// ============================================================

// ========== ProgDesign 程设 ==========
void ProgDesignSubject::interact() {
    qDebug() << "[ProgDesign] interact, current affinity =" << affinity_;
    // 信号会被 MainWindow 监听，触发对应剧本加载
    // 根据当前好感度选不同的剧本：
    //   - affinity < 30: route_progdesign.json 早期对话
    //   - affinity >= 60: route_progdesign.json 中后期对话
    emit interactionRequested(SubjectType::ProgDesign);
}

MiniGame* ProgDesignSubject::createGame() {
    // 程设有两个小游戏：扫雷 和 21 点
    // 简单起见默认返回扫雷；扩展点：根据剧情上下文返回 21 点
    return new MinesweeperGame;
}

// ========== Calculus 高数 ==========
void CalculusSubject::interact() {
    qDebug() << "[Calculus] interact, current affinity =" << affinity_;
    emit interactionRequested(SubjectType::Calculus);
}

MiniGame* CalculusSubject::createGame() {
    return new BlackjackGame;
}

// ========== LinearAlgebra 线代 ==========
void LinearAlgebraSubject::interact() {
    qDebug() << "[LinearAlgebra] interact, affinity =" << affinity_;
    // 线代角色比较特殊：黑化值高时台词会变得偏执
    // 这部分逻辑由 JSON 剧本中的 condition 字段控制（如 "darkness >= 30"）
    emit interactionRequested(SubjectType::LinearAlgebra);
}

MiniGame* LinearAlgebraSubject::createGame() {
    return new MemoryGame;
}

// ========== AIIntro AI引论 ==========
void AIIntroSubject::interact() {
    qDebug() << "[AIIntro] interact, current affinity =" << affinity_;
    emit interactionRequested(SubjectType::AIIntro);
}

MiniGame* AIIntroSubject::createGame() {
    // AI 引论有两个小游戏：井字棋 和 AI 迷宫
    // 默认返回井字棋；进阶剧情可改为迷宫
    return new TicTacToeGame;
}

} // namespace SA
