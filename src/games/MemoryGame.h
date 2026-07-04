/**
 * @file MemoryGame.h
 * @brief 记忆翻牌小游戏（对应 程设学科）
 *
 * 玩法：点击翻牌，匹配相同数字
 *
 * 负责人：成员 C   预计完成：5/24
 */
#ifndef MEMORYGAME_H
#define MEMORYGAME_H

#include "../common/MiniGame.h"
#include <QPushButton>
#include <QGridLayout>
#include <QVector>
#include <QStackedLayout>
#include <QLabel>
#include <QTimer>

namespace SA {

class MemoryGame : public MiniGame
{
    Q_OBJECT

public:
    explicit MemoryGame(QWidget* parent = nullptr);

    void start() override;

    void reset() override;

    MiniGameType type() const override;

    QString displayName() const override;

private slots:
    void handleCardClicked();

private:
    QVector<QPushButton*> cards_;

    QVector<int> values_;

    QPushButton* firstCard_ = nullptr;

    QPushButton* secondCard_ = nullptr;

    int matchedPairs_ = 0;

    bool busy_ = false;

    void setupBoard();

    QStackedLayout* stackedLayout_;

    QWidget* introPage_;
    QWidget* gamePage_;

    QLabel* timerLabel_;

    QTimer* gameTimer_;

    int remainingTime_;
};

}

#endif