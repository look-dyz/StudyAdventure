#include "MainMenu.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>

namespace SA {

MainMenu::MainMenu(QWidget* parent) : QWidget(parent) {
    // 背景图 Label（铺满）
    auto* bgLabel = new QLabel(this);
    bgLabel->setObjectName("bgLabel");
    bgLabel->setScaledContents(true);
    QPixmap bg(":/images/backgrounds/主页面背景图.jpg");
    bgLabel->setPixmap(bg);
    bgLabel->lower();

    auto* layout = new QVBoxLayout(this);

    auto* title = new QLabel(tr("学海漫游\n异世界的信科少女"), this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-size: 36px; font-weight: bold; color: #FFFFFF;"
        "background: rgba(0,0,0,120); border-radius: 12px; padding: 16px;"
        );

    auto* startBtn    = new QPushButton(tr("开始游戏"), this);
    auto* loadBtn     = new QPushButton(tr("读取存档"), this);
    auto* settingsBtn = new QPushButton(tr("设置"), this);
    auto* exitBtn     = new QPushButton(tr("退出"), this);

    QString btnStyle =
        "QPushButton {"
        "  font-size: 18px;"
        "  background: rgba(255,255,255,180);"
        "  border-radius: 8px;"
        "  color: #4A0000;"
        "  padding: 6px 0;"
        "}"
        "QPushButton:hover {"
        "  background: rgba(255,220,180,220);"
        "}";

    for (auto* btn : {startBtn, loadBtn, settingsBtn, exitBtn}) {
        btn->setMinimumHeight(48);
        btn->setStyleSheet(btnStyle);
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

void MainMenu::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    // 背景图跟随窗口大小
    if (auto* bg = findChild<QLabel*>("bgLabel")) {
        bg->setGeometry(0, 0, width(), height());
    }
}

} // namespace SA