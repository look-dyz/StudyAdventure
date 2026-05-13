#include "MainWindow.h"
#include "core/GameManager.h"
#include "story/DialogWindow.h"

#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

namespace SA {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      stack_(new QStackedWidget(this))
{
    setWindowTitle(tr("学海漫游：异世界的信科少女"));
    resize(1024, 768);

    setupUi();
    connectSignals();

    // 初始进入主菜单
    stack_->setCurrentIndex(mainMenuIndex_);
}

void MainWindow::setupUi() {
    // ========== 页面 0：主菜单 ==========
    auto* mainMenuPage = new QWidget;
    {
        auto* layout = new QVBoxLayout(mainMenuPage);
        auto* title = new QLabel(tr("学海漫游\n异世界的信科少女"));
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet("font-size: 36px; font-weight: bold; color: #8B1A1A;");

        auto* startBtn = new QPushButton(tr("开始游戏"));
        auto* loadBtn  = new QPushButton(tr("读取存档"));
        auto* exitBtn  = new QPushButton(tr("退出"));
        for (auto* btn : {startBtn, loadBtn, exitBtn}) {
            btn->setMinimumHeight(48);
            btn->setStyleSheet("font-size: 18px;");
        }

        layout->addStretch();
        layout->addWidget(title);
        layout->addSpacing(40);
        layout->addWidget(startBtn);
        layout->addWidget(loadBtn);
        layout->addWidget(exitBtn);
        layout->addStretch();
        layout->setContentsMargins(200, 80, 200, 80);

        connect(startBtn, &QPushButton::clicked, this, []() {
            GameManager::instance().requestScene(GameScene::Map);
        });
        connect(exitBtn, &QPushButton::clicked, this, &QMainWindow::close);
    }
    stack_->addWidget(mainMenuPage);

    // ========== 页面 1：地图（占位）==========
    auto* mapPage = new QWidget;
    {
        auto* layout = new QVBoxLayout(mapPage);
        auto* label = new QLabel(tr("[ 地图场景 ]\n\n（成员 B 待实现：MapScene + 4 个地点）"));
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-size: 24px; color: #555;");

        auto* dialogBtn = new QPushButton(tr("→ 测试：进入对话"));
        auto* gameBtn   = new QPushButton(tr("→ 测试：进入小游戏"));
        auto* backBtn   = new QPushButton(tr("← 返回主菜单"));

        layout->addStretch();
        layout->addWidget(label);
        layout->addSpacing(40);
        layout->addWidget(dialogBtn);
        layout->addWidget(gameBtn);
        layout->addWidget(backBtn);
        layout->addStretch();
        layout->setContentsMargins(200, 40, 200, 40);

        connect(dialogBtn, &QPushButton::clicked, this, []() {
            GameManager::instance().requestScene(GameScene::Dialog);
        });
        connect(gameBtn, &QPushButton::clicked, this, []() {
            GameManager::instance().requestScene(GameScene::MiniGame);
        });
        connect(backBtn, &QPushButton::clicked, this, []() {
            GameManager::instance().requestScene(GameScene::MainMenu);
        });
    }
    stack_->addWidget(mapPage);

    // ========== 页面 2：对话（占位）==========
    auto* dialogPage = new QWidget;
    {
        auto* layout = new QVBoxLayout(dialogPage);
        auto* dialog = new DialogWindow;
        dialog->setContent(
            tr("程设"),
            tr("欢迎来到异世界。我是程序设计——你接下来一学期的同伴。\n（此处为占位剧情，成员 A 将通过 StoryEngine 加载真实剧本）"),
            {tr("……你好"), tr("我要回去！")}
        );

        auto* backBtn = new QPushButton(tr("← 返回地图"));
        connect(dialog, &DialogWindow::choiceMade, this, [](int idx) {
            Q_UNUSED(idx);
            GameManager::instance().requestScene(GameScene::Map);
        });
        connect(backBtn, &QPushButton::clicked, this, []() {
            GameManager::instance().requestScene(GameScene::Map);
        });

        layout->addWidget(dialog);
        layout->addWidget(backBtn);
        layout->setContentsMargins(60, 40, 60, 40);
    }
    stack_->addWidget(dialogPage);

    // ========== 页面 3：小游戏（占位）==========
    auto* miniGamePage = new QWidget;
    {
        auto* layout = new QVBoxLayout(miniGamePage);
        auto* label = new QLabel(tr("[ 小游戏区 ]\n\n（成员 C 待实现：6 个小游戏）"));
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-size: 24px; color: #555;");

        auto* backBtn = new QPushButton(tr("← 返回地图"));
        connect(backBtn, &QPushButton::clicked, this, []() {
            GameManager::instance().requestScene(GameScene::Map);
        });

        layout->addStretch();
        layout->addWidget(label);
        layout->addSpacing(40);
        layout->addWidget(backBtn);
        layout->addStretch();
        layout->setContentsMargins(200, 40, 200, 40);
    }
    stack_->addWidget(miniGamePage);

    // ========== 页面 4：结局（占位）==========
    auto* endingPage = new QWidget;
    {
        auto* layout = new QVBoxLayout(endingPage);
        auto* label = new QLabel(tr("[ 结局演出 ]"));
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-size: 32px; color: #8B1A1A;");

        layout->addWidget(label);
        layout->setContentsMargins(60, 60, 60, 60);
    }
    stack_->addWidget(endingPage);

    setCentralWidget(stack_);
}

void MainWindow::connectSignals() {
    connect(&GameManager::instance(), &GameManager::sceneChangeRequested,
            this, &MainWindow::onSceneChangeRequested);
}

void MainWindow::onSceneChangeRequested(GameScene scene) {
    switch (scene) {
        case GameScene::MainMenu: stack_->setCurrentIndex(mainMenuIndex_); break;
        case GameScene::Map:      stack_->setCurrentIndex(mapIndex_); break;
        case GameScene::Dialog:   stack_->setCurrentIndex(dialogIndex_); break;
        case GameScene::MiniGame: stack_->setCurrentIndex(miniGameIndex_); break;
        case GameScene::Ending:   stack_->setCurrentIndex(endingIndex_); break;
    }
}

} // namespace SA
