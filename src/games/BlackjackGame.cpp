#include "BlackjackGame.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <random>
#include <algorithm>

namespace SA {

// ========== Card 实现 ==========
int Card::blackjackValue() const {
    if (rank >= 11) return 10;   // J/Q/K
    if (rank == 1) return 1;     // A 默认按 1，hand 计算时再考虑 +10
    return rank;
}

QString Card::display() const {
    static const QString suitChars[] = {"♠", "♥", "♣", "♦"};
    QString r;
    switch (rank) {
        case 1:  r = "A"; break;
        case 11: r = "J"; break;
        case 12: r = "Q"; break;
        case 13: r = "K"; break;
        default: r = QString::number(rank);
    }
    return suitChars[suit] + r;
}

// ========== Deck 实现 ==========
Deck::Deck() {
    cards_.reserve(52);
    for (int s = 0; s < 4; ++s) {
        for (int r = 1; r <= 13; ++r) {
            cards_.push_back({static_cast<Card::Suit>(s), r});
        }
    }
    shuffle();
}

void Deck::shuffle() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::shuffle(cards_.begin(), cards_.end(), gen);
}

Card Deck::draw() {
    Card c = cards_.back();
    cards_.pop_back();
    return c;
}

// ========== BlackjackGame 实现 ==========
BlackjackGame::BlackjackGame(QWidget* parent) : MiniGame(parent) {
    auto* layout = new QVBoxLayout(this);

    auto* title = new QLabel(displayName(), this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #8B1A1A;");

    dealerLabel_ = new QLabel(this);
    playerLabel_ = new QLabel(this);
    statusLabel_ = new QLabel(this);
    statusLabel_->setStyleSheet("font-size: 16px; color: #3B5BA5;");

    hitBtn_ = new QPushButton(tr("要牌"), this);
    standBtn_ = new QPushButton(tr("停牌"), this);
    connect(hitBtn_, &QPushButton::clicked, this, &BlackjackGame::onHitClicked);
    connect(standBtn_, &QPushButton::clicked, this, &BlackjackGame::onStandClicked);

    auto* btnRow = new QHBoxLayout;
    btnRow->addWidget(hitBtn_);
    btnRow->addWidget(standBtn_);

    layout->addWidget(title);
    layout->addWidget(dealerLabel_);
    layout->addWidget(playerLabel_);
    layout->addWidget(statusLabel_);
    layout->addLayout(btnRow);
    layout->addStretch();
}

void BlackjackGame::start() {
    reset();
}

void BlackjackGame::reset() {
    deck_ = Deck();
    playerHand_.clear();
    dealerHand_.clear();
    dealerHidden_ = true;
    gameEnded_ = false;

    // 初始各发两张
    playerHand_.push_back(deck_.draw());
    playerHand_.push_back(deck_.draw());
    dealerHand_.push_back(deck_.draw());
    dealerHand_.push_back(deck_.draw());

    hitBtn_->setEnabled(true);
    standBtn_->setEnabled(true);
    statusLabel_->setText(tr("你的回合"));

    // 检查初始 Blackjack
    if (calculateScore(playerHand_) == 21) {
        dealerHidden_ = false;
        endGame(true, true);
    }
    updateDisplay();
}

int BlackjackGame::calculateScore(const std::vector<Card>& hand) const {
    int sum = 0;
    int aceCount = 0;
    for (const auto& c : hand) {
        sum += c.blackjackValue();
        if (c.rank == 1) aceCount++;
    }
    // A 计 11 不爆牌则升级
    while (aceCount > 0 && sum + 10 <= 21) {
        sum += 10;
        aceCount--;
    }
    return sum;
}

void BlackjackGame::onHitClicked() {
    if (gameEnded_) return;
    playerHand_.push_back(deck_.draw());
    updateDisplay();
    if (calculateScore(playerHand_) > 21) {
        dealerHidden_ = false;
        endGame(false);
    }
}

void BlackjackGame::onStandClicked() {
    if (gameEnded_) return;
    dealerHidden_ = false;
    dealerTurn();
}

void BlackjackGame::dealerTurn() {
    while (calculateScore(dealerHand_) < 17) {
        dealerHand_.push_back(deck_.draw());
    }
    int playerScore = calculateScore(playerHand_);
    int dealerScore = calculateScore(dealerHand_);

    bool win = false;
    if (dealerScore > 21) win = true;
    else if (playerScore > dealerScore) win = true;

    endGame(win);
}

void BlackjackGame::updateDisplay() {
    QString p = tr("你（%1）: ").arg(calculateScore(playerHand_));
    for (const auto& c : playerHand_) p += c.display() + " ";

    QString d;
    if (dealerHidden_) {
        d = tr("庄家（?）: %1  [?]").arg(dealerHand_[0].display());
    } else {
        d = tr("庄家（%1）: ").arg(calculateScore(dealerHand_));
        for (const auto& c : dealerHand_) d += c.display() + " ";
    }

    dealerLabel_->setText(d);
    playerLabel_->setText(p);
}

void BlackjackGame::endGame(bool playerWon, bool blackjack) {
    gameEnded_ = true;
    hitBtn_->setEnabled(false);
    standBtn_->setEnabled(false);
    updateDisplay();

    int score = 0;
    if (blackjack) {
        score = 100;
        statusLabel_->setText(tr("Blackjack！完胜！"));
    } else if (playerWon) {
        score = 70;
        statusLabel_->setText(tr("你赢了！"));
    } else {
        score = 20;
        statusLabel_->setText(tr("你输了……"));
    }
    emit finished(score, playerWon);
}

} // namespace SA
