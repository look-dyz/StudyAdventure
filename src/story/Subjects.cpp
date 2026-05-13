#include "Subjects.h"
#include "games/MinesweeperGame.h"
#include "games/BlackjackGame.h"
#include "games/TicTacToeGame.h"
#include "games/MatrixGame.h"
#include "games/MazeGame.h"
#include "games/CalculusGame.h"
#include <QDebug>

namespace SA {

// ========== ProgDesign ==========
void ProgDesignSubject::interact() {
    // TODO（成员 A）：触发与程设的对话剧情
    qDebug() << "[ProgDesign] interact, affinity =" << affinity_;
}

MiniGame* ProgDesignSubject::createGame() {
    // 简单实现：默认扫雷；后续可根据剧情上下文返回 21 点
    return new MinesweeperGame;
}

// ========== Calculus ==========
void CalculusSubject::interact() {
    // TODO（成员 A）
    qDebug() << "[Calculus] interact, affinity =" << affinity_;
}

MiniGame* CalculusSubject::createGame() {
    return new CalculusGame;
}

// ========== LinearAlgebra ==========
void LinearAlgebraSubject::interact() {
    // TODO（成员 A）：注意黑化倾向台词的层次
    qDebug() << "[LinearAlgebra] interact, affinity =" << affinity_;
}

MiniGame* LinearAlgebraSubject::createGame() {
    return new MatrixGame;
}

// ========== AIIntro ==========
void AIIntroSubject::interact() {
    // TODO（成员 A）
    qDebug() << "[AIIntro] interact, affinity =" << affinity_;
}

MiniGame* AIIntroSubject::createGame() {
    // 简单实现：默认井字棋；后续可根据剧情上下文返回迷宫
    return new TicTacToeGame;
}

} // namespace SA
