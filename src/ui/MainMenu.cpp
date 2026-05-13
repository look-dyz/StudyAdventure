#include "MainMenu.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace SA {

MainMenu::MainMenu(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);

    auto* title = new QLabel(tr("学海漫游\n异世界的信科少女"), this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 36px; font-weight: bold; color: #8B1A1A;");

    auto* startBtn    = new QPushButton(tr("开始游戏"), this);
    auto* loadBtn     = new QPushButton(tr("读取存档"), this);
    auto* settingsBtn = new QPushButton(tr("设置"), this);
    auto* exitBtn     = new QPushButton(tr("退出"), this);

    for (auto* btn : {startBtn, loadBtn, settingsBtn, exitBtn}) {
        btn->setMinimumHeight(48);
        btn->setStyleSheet("font-size: 18px;");
    }

    layout->addStretch();
    layout->addWidget(title);
    layout->addSpacing(40);
    layout->addWidget(startBtn);
    layout->addWidget(loadBtn);
    layout->addWidget(settingsBtn);
    layout->addWidget(exitBtn);
    layout->addStretch();
    layout->setContentsMargins(200, 80, 200, 80);

    connect(startBtn,    &QPushButton::clicked, this, &MainMenu::startNewGameClicked);
    connect(loadBtn,     &QPushButton::clicked, this, &MainMenu::loadGameClicked);
    connect(settingsBtn, &QPushButton::clicked, this, &MainMenu::settingsClicked);
    connect(exitBtn,     &QPushButton::clicked, this, &MainMenu::exitClicked);
}

} // namespace SA
