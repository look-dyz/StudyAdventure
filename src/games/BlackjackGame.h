/**
 * @file BlackjackGame.h
 * @brief 21 点小游戏（对应程设学科）
 *
 * 玩法：玩家 vs 庄家，标准 21 点规则。
 * OOP 重点：Card / Deck 类的封装，运算符重载练习。
 *
 * 负责人：成员 C   预计完成：5/20
 */
#ifndef STUDYADVENTURE_BLACKJACKGAME_H
#define STUDYADVENTURE_BLACKJACKGAME_H

#include "common/MiniGame.h"
#include <QString>
#include <vector>
#include <QStackedLayout>

class QLabel;
class QPushButton;
class QHBoxLayout;
class QStackedLayout;
class QWidget;

namespace SA {

// ========== Card 类 ==========
struct Card {
    enum Suit { Spade, Heart, Club, Diamond };
    Suit suit;
    int rank;  ///< 1=A, 2-10, 11=J, 12=Q, 13=K

    int blackjackValue() const;  ///< J/Q/K=10, A=1（A 的灵活计数在 hand 层处理）
    QString display() const;
};

// ========== Deck 类 ==========
class Deck {
public:
    Deck();              ///< 构造 52 张并洗牌
    void shuffle();
    Card draw();
    bool empty() const { return cards_.empty(); }
private:
    std::vector<Card> cards_;
};

// ========== 主游戏类 ==========
class BlackjackGame : public MiniGame {
    Q_OBJECT
public:
    explicit BlackjackGame(QWidget* parent = nullptr);
    ~BlackjackGame() override = default;

    void start() override;
    void reset() override;
    MiniGameType type() const override { return MiniGameType::Blackjack; }
    QString displayName() const override { return tr("21 点"); }

private slots:
    void onHitClicked();
    void onStandClicked();

private:
    Deck deck_;
    std::vector<Card> playerHand_;
    std::vector<Card> dealerHand_;
    bool dealerHidden_ = true;
    bool gameEnded_ = false;

    QStackedLayout* stackedLayout_ = nullptr;

    QWidget* introPage_ = nullptr;

    QWidget* gamePage_ = nullptr;

    // UI 部件
    QLabel* statusLabel_ = nullptr;
    QLabel* playerLabel_ = nullptr;
    QLabel* dealerLabel_ = nullptr;
    QPushButton* hitBtn_ = nullptr;
    QPushButton* standBtn_ = nullptr;
    QPushButton* resetBtn_;

    int calculateScore(const std::vector<Card>& hand) const;  ///< 含 A 灵活计数
    void updateDisplay();
    void dealerTurn();
    void endGame(bool playerWon, bool blackjack = false);
};

} // namespace SA

#endif // STUDYADVENTURE_BLACKJACKGAME_H
