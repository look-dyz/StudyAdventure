#include "StatusBar.h"
#include "core/Player.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>

namespace SA {

StatusBar::StatusBar(Player* player, QWidget* parent) : QWidget(parent) {
    buildUI();

    // 连接信号 - 这就是"模块解耦"的体现：StatusBar 只通过信号了解 Player 变化
    connect(player, &Player::affinityChanged, this, &StatusBar::onAffinityChanged);
    connect(player, &Player::stressChanged,   this, &StatusBar::onStressChanged);
    connect(player, &Player::darknessChanged, this, &StatusBar::onDarknessChanged);
    connect(player, &Player::dateChanged,     this, &StatusBar::onDateChanged);

    // 初始化显示
    for (auto s : {SubjectType::ProgDesign, SubjectType::Calculus,
                   SubjectType::LinearAlgebra, SubjectType::AIIntro}) {
        onAffinityChanged(s, player->affinity(s));
    }
    onStressChanged(player->stress());
    onDarknessChanged(player->darkness());
    onDateChanged(player->currentWeek(), player->currentDay());
}

QProgressBar* StatusBar::createBar(const QString& color) {
    auto* bar = new QProgressBar(this);
    bar->setRange(0, 100);
    bar->setTextVisible(true);
    bar->setMaximumHeight(20);
    bar->setStyleSheet(QString(
        "QProgressBar { border: 1px solid #ccc; border-radius: 3px; text-align: center; }"
        "QProgressBar::chunk { background-color: %1; }").arg(color));
    return bar;
}

void StatusBar::buildUI() {
    auto* mainLayout = new QHBoxLayout(this);

    // 日期
    dateLabel_ = new QLabel(this);
    dateLabel_->setStyleSheet("font-weight: bold; font-size: 14px;");

    // 好感度（4 个）
    auto* affinityLayout = new QGridLayout;
    int row = 0;
    for (auto s : {SubjectType::ProgDesign, SubjectType::Calculus,
                   SubjectType::LinearAlgebra, SubjectType::AIIntro}) {
        auto* lbl = new QLabel(subjectName(s) + ":", this);
        auto* bar = createBar("#FF69B4");
        affinityBars_[s] = bar;
        affinityLayout->addWidget(lbl, row / 2, (row % 2) * 2);
        affinityLayout->addWidget(bar, row / 2, (row % 2) * 2 + 1);
        row++;
    }

    // 压力 + 黑化
    auto* extraLayout = new QVBoxLayout;
    auto* stressRow = new QHBoxLayout;
    stressRow->addWidget(new QLabel(tr("压力:"), this));
    stressBar_ = createBar("#3B5BA5");
    stressRow->addWidget(stressBar_);

    auto* darknessRow = new QHBoxLayout;
    darknessRow->addWidget(new QLabel(tr("黑化:"), this));
    darknessBar_ = createBar("#5A189A");
    darknessRow->addWidget(darknessBar_);

    extraLayout->addLayout(stressRow);
    extraLayout->addLayout(darknessRow);

    mainLayout->addWidget(dateLabel_);
    mainLayout->addLayout(affinityLayout, 2);
    mainLayout->addLayout(extraLayout, 1);
    mainLayout->setContentsMargins(8, 4, 8, 4);
}

void StatusBar::onAffinityChanged(SubjectType s, int value) {
    if (affinityBars_.contains(s)) {
        affinityBars_[s]->setValue(value);
    }
}

void StatusBar::onStressChanged(int value) {
    stressBar_->setValue(value);
}

void StatusBar::onDarknessChanged(int value) {
    darknessBar_->setValue(value);
}

void StatusBar::onDateChanged(int week, int day) {
    dateLabel_->setText(tr("第 %1 周·第 %2 天").arg(week).arg(day));
}

} // namespace SA
