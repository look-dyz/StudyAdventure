#include "BlackjackGame.h"
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <random>
#include <algorithm>
#include <QStackedLayout>
#include <QMessageBox>

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

    if(cards_.empty()) {

        qDebug() << "[Deck] Empty deck! Reshuffling.";

        *this = Deck();
    }

    Card c = cards_.back();

    cards_.pop_back();

    return c;
}

// ========== BlackjackGame 实现 ==========
BlackjackGame::BlackjackGame(QWidget* parent) : MiniGame(parent) {
    stackedLayout_ = new QStackedLayout(this);

    introPage_ = new QWidget(this);

    {
        auto* layout =
            new QVBoxLayout(introPage_);

        auto* title =
            new QLabel(tr("21点玩法说明"));

        title->setAlignment(Qt::AlignCenter);

        title->setStyleSheet(
            "font-size:24px;"
            "font-weight:bold;"
            "color:#8B1A1A;"
            );

        auto* introText =
            new QLabel(
                tr(
                    "【游戏目标】\n"
                    "你的点数尽量接近 21，"
                    "但不能超过 21。\n\n"

                    "【规则】\n"
                    "数字牌按原值计算。\n"
                    "J/Q/K 都算 10 点。\n"
                    "A 可算 1 或 11。\n\n"

                    "【操作】\n"
                    "要牌：继续抽牌。\n"
                    "停牌：结束回合，由庄家行动。\n\n"

                    "【胜负】\n"
                    "超过 21 点直接失败。\n"
                    "最终点数更接近 21 的获胜。"
                    )
                );

        introText->setWordWrap(true);

        introText->setStyleSheet(
            "font-size:16px;"
            "padding:20px;"
            );

        auto* startBtn =
            new QPushButton(tr("开始游戏"));

        startBtn->setMinimumHeight(50);

        startBtn->setStyleSheet(
            "font-size:18px;"
            "background:#F5E6CA;"
            "border-radius:10px;"
            );

        connect(startBtn,
                &QPushButton::clicked,
                this,
                [this]() {

                    stackedLayout_->setCurrentWidget(gamePage_);

                    reset();
                });

        layout->addStretch();

        layout->addWidget(title);

        layout->addWidget(introText);

        layout->addWidget(startBtn);

        layout->addStretch();
    }
    // =====================================================
    // 正式游戏页
    // =====================================================

    gamePage_ = new QWidget(this);

    {
        auto* layout =
            new QVBoxLayout(gamePage_);

        auto* title =
            new QLabel(displayName(), gamePage_);

        title->setAlignment(Qt::AlignCenter);

        title->setStyleSheet(
            "font-size: 20px;"
            "font-weight: bold;"
            "color: #8B1A1A;"
            );

        dealerLabel_ = new QLabel(gamePage_);

        playerLabel_ = new QLabel(gamePage_);

        statusLabel_ = new QLabel(gamePage_);

        statusLabel_->setStyleSheet(
            "font-size:16px;"
            "color:#3B5BA5;"
            );

        hitBtn_ =
            new QPushButton(tr("要牌"), gamePage_);

        standBtn_ =
            new QPushButton(tr("停牌"), gamePage_);

        resetBtn_ =
            new QPushButton(tr("重新开始"), gamePage_);

        resetBtn_->hide();

        connect(hitBtn_,
                &QPushButton::clicked,
                this,
                &BlackjackGame::onHitClicked);

        connect(standBtn_,
                &QPushButton::clicked,
                this,
                &BlackjackGame::onStandClicked);

        connect(resetBtn_,
                &QPushButton::clicked,
                this,
                &BlackjackGame::reset);

        auto* btnRow =
            new QHBoxLayout;

        btnRow->addWidget(hitBtn_);

        btnRow->addWidget(standBtn_);

        btnRow->addWidget(resetBtn_);

        layout->addWidget(title);

        layout->addWidget(dealerLabel_);

        layout->addWidget(playerLabel_);

        layout->addWidget(statusLabel_);

        layout->addLayout(btnRow);

        layout->addStretch();
    }

    stackedLayout_->addWidget(introPage_);
    stackedLayout_->addWidget(gamePage_);

    stackedLayout_->setCurrentWidget(introPage_);
}

void BlackjackGame::start() {

    stackedLayout_->setCurrentWidget(introPage_);
}

void BlackjackGame::reset() {
    resetBtn_->hide();
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
    statusLabel_->setText(
        tr("你的回合：请选择【要牌】或【停牌】")
        );

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
    QString p =
        tr("你的手牌（当前 %1 点）:\n")
            .arg(calculateScore(playerHand_));
    for (const auto& c : playerHand_) p += c.display() + " ";

    QString d;
    if (dealerHidden_) {
        d = tr("庄家手牌（总点数未知）: %1  [未知牌]").arg(dealerHand_[0].display());
    } else {
        d = tr("庄家手牌（当前 %1 点）: ").arg(calculateScore(dealerHand_));
        for (const auto& c : dealerHand_) d += c.display() + " ";
    }

    dealerLabel_->setText(d);
    playerLabel_->setText(p);
}

void BlackjackGame::endGame(bool playerWon, bool blackjack) {

    gameEnded_ = true;

    hitBtn_->setEnabled(false);

    standBtn_->setEnabled(false);

    resetBtn_->show();

    updateDisplay();

    int score = 0;

    QString message;

    if (blackjack) {

        score = 100;

        message = tr("Blackjack！完美21点！\n你获得了最高奖励！");

        statusLabel_->setText(
            tr("Blackjack！完胜！")
            );

    }
    else if (playerWon) {

        score = 70;

        message = tr("恭喜你获胜！");

        statusLabel_->setText(
            tr("你赢了！")
            );
    }
    else {

        score = 20;

        message = tr("很遗憾，你输了……");

        statusLabel_->setText(
            tr("你输了……")
            );
    }

            // =========================
            // 弹窗
            // =========================

    QMessageBox::information(
        this,
        tr("游戏结束"),
        message
        );

    emit finished(score, playerWon);
}

} // namespace SA
