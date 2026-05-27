#include "MemoryGame.h"

#include <QVBoxLayout>
#include <QTimer>
#include <QRandomGenerator>
#include <algorithm>

#include <QStackedLayout>
#include <QLabel>
#include <QTimer>

namespace SA {

MemoryGame::MemoryGame(QWidget* parent)
    : MiniGame(parent)
{
    stackedLayout_ = new QStackedLayout(this);

    // =====================================
    // 说明页面
    // =====================================

    introPage_ = new QWidget(this);

    {
        auto* layout =
            new QVBoxLayout(introPage_);

        auto* title =
            new QLabel(tr("线代记忆训练"));

        title->setAlignment(Qt::AlignCenter);

        title->setStyleSheet(
            "font-size:28px;"
            "font-weight:bold;"
            "color:#6A3D9A;"
            );

        auto* intro =
            new QLabel(
                tr(
                    "【规则说明】\n\n"
                    "在限定时间内完成所有配对。\n"
                    "卡牌内容与线性代数知识相关。\n\n"
                    "若时间结束仍未完成，则挑战失败。\n\n"
                    "失败会大幅增加线代黑化值。"
                    )
                );

        intro->setWordWrap(true);

        intro->setStyleSheet(
            "font-size:18px;"
            "padding:20px;"
            );

        auto* startBtn =
            new QPushButton(tr("开始训练"));

        startBtn->setMinimumHeight(60);

        startBtn->setStyleSheet(
            "font-size:20px;"
            "background:#E8D9FF;"
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

        layout->addWidget(intro);

        layout->addWidget(startBtn);

        layout->addStretch();
    }

    // =====================================
    // 游戏页面
    // =====================================

    gamePage_ = new QWidget(this);

    {
        auto* layout =
            new QVBoxLayout(gamePage_);

        timerLabel_ =
            new QLabel;

        timerLabel_->setAlignment(Qt::AlignCenter);

        timerLabel_->setStyleSheet(
            "font-size:22px;"
            "color:red;"
            "font-weight:bold;"
            );

        layout->addWidget(timerLabel_);

        auto* grid =
            new QGridLayout;

        layout->addLayout(grid);

        for(int i = 0; i < 16; ++i) {

            auto* btn =
                new QPushButton("?");

            btn->setFixedSize(90, 90);

            btn->setStyleSheet(
                "font-size:18px;"
                );

            connect(btn,
                    &QPushButton::clicked,
                    this,
                    &MemoryGame::handleCardClicked);

            cards_.push_back(btn);

            grid->addWidget(btn,
                            i / 4,
                            i % 4);
        }
    }

    // =====================================

    stackedLayout_->addWidget(introPage_);

    stackedLayout_->addWidget(gamePage_);

    stackedLayout_->setCurrentWidget(introPage_);

    gameTimer_ = new QTimer(this);

    connect(gameTimer_,
            &QTimer::timeout,
            this,
            [this]() {

                remainingTime_--;

                timerLabel_->setText(
                    tr("剩余时间：%1 秒")
                    .arg(remainingTime_)
                    );

                if(remainingTime_ <= 0) {

                    gameTimer_->stop();

                    emit finished(20, false);
                }
            });

    setupBoard();
}

void MemoryGame::start()
{
    stackedLayout_->setCurrentWidget(introPage_);
}

void MemoryGame::reset()
{
    matchedPairs_ = 0;

    firstCard_ = nullptr;

    secondCard_ = nullptr;

    busy_ = false;

    remainingTime_ = 60;

    timerLabel_->setText(
        tr("剩余时间：60 秒")
        );

    gameTimer_->start(1000);

    setupBoard();
}

MiniGameType MemoryGame::type() const
{
    return MiniGameType::MemoryMatch;
}

QString MemoryGame::displayName() const
{
    return tr("记忆翻牌");
}

void MemoryGame::setupBoard()
{
    values_.clear();

    for(int i = 1; i <= 8; ++i) {

        values_.push_back(i);

        values_.push_back(i);
    }

    std::shuffle(values_.begin(),
                 values_.end(),
                 *QRandomGenerator::global());

    for(int i = 0; i < cards_.size(); ++i) {

        cards_[i]->setText("?");

        cards_[i]->setEnabled(true);

        cards_[i]->setProperty("value", values_[i]);
    }
}

void MemoryGame::handleCardClicked()
{
    if(busy_) return;

    auto* btn = qobject_cast<QPushButton*>(sender());

    if(!btn) return;

    if(btn == firstCard_) return;

    int value = btn->property("value").toInt();

    btn->setText(QString::number(value));

    if(!firstCard_) {

        firstCard_ = btn;

        return;
    }

    secondCard_ = btn;

    int firstValue =
        firstCard_->property("value").toInt();

    int secondValue =
        secondCard_->property("value").toInt();

    // 配对成功
    if(firstValue == secondValue) {

        firstCard_->setEnabled(false);

        secondCard_->setEnabled(false);

        firstCard_ = nullptr;

        secondCard_ = nullptr;

        matchedPairs_++;

        // 全部完成
        if(matchedPairs_ == 8) {
            gameTimer_->stop();
            emit finished(100, true);
        }

        return;
    }

    // 配对失败
    busy_ = true;

    QTimer::singleShot(800,
                       this,
                       [this]() {

        firstCard_->setText("?");

        secondCard_->setText("?");

        firstCard_ = nullptr;

        secondCard_ = nullptr;

        busy_ = false;
    });
}

}