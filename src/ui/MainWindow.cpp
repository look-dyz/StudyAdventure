#include "MainWindow.h"
#include "MainMenu.h"
#include "StatusBar.h"
#include "core/GameManager.h"
#include "core/Player.h"
#include "story/DialogWindow.h"
#include "games/TicTacToeGame.h"

#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QApplication>

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
    auto* central = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // ========== 顶部状态栏（始终显示）==========
    auto* statusBar = new StatusBar(GameManager::instance().player(), this);
    mainLayout->addWidget(statusBar);

    // ========== 中间堆栈 ==========
    // 页面 0：主菜单
    auto* mainMenu = new MainMenu;
    connect(mainMenu, &MainMenu::startNewGameClicked, this, []() {
        GameManager::instance().requestScene(GameScene::Map);
    });
    connect(mainMenu, &MainMenu::loadGameClicked, this, []() {
        // TODO（成员 C）：弹出存档槽选择
    });
    connect(mainMenu, &MainMenu::settingsClicked, this, []() {
        // TODO（成员 B）：设置面板
    });
    connect(mainMenu, &MainMenu::exitClicked, this, []() {
        QApplication::quit();
    });
    stack_->addWidget(mainMenu);

    // 页面 1：地图（占位）
    auto* mapPage = new QWidget;
    {
        auto* layout = new QVBoxLayout(mapPage);
        auto* label = new QLabel(tr("[ 地图场景 ]\n\n（成员 B 待实现：MapScene + 4 个地点）"));
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-size: 24px; color: #555;");

        auto* dialogBtn = new QPushButton(tr("→ 测试：进入对话"));
        auto* gameBtn   = new QPushButton(tr("→ 测试：玩井字棋（演示）"));
        auto* tickBtn   = new QPushButton(tr("→ 测试：推进一天 + 加好感"));
        auto* backBtn   = new QPushButton(tr("← 返回主菜单"));

        layout->addStretch();
        layout->addWidget(label);
        layout->addSpacing(20);
        layout->addWidget(dialogBtn);
        layout->addWidget(gameBtn);
        layout->addWidget(tickBtn);
        layout->addWidget(backBtn);
        layout->addStretch();
        layout->setContentsMargins(200, 40, 200, 40);

        connect(dialogBtn, &QPushButton::clicked, this, []() {
            GameManager::instance().requestScene(GameScene::Dialog);
        });
        connect(gameBtn, &QPushButton::clicked, this, []() {
            GameManager::instance().requestScene(GameScene::MiniGame);
        });
        connect(tickBtn, &QPushButton::clicked, this, []() {
            auto* player = GameManager::instance().player();
            player->advanceDay();
            player->addAffinity(SubjectType::ProgDesign, 3);
            player->addStress(2);
        });
        connect(backBtn, &QPushButton::clicked, this, []() {
            GameManager::instance().requestScene(GameScene::MainMenu);
        });
    }
    stack_->addWidget(mapPage);

    // 页面 2：对话（占位）
    auto* dialogPage = new QWidget;
    {
        auto* layout = new QVBoxLayout(dialogPage);
        auto* dialog = new DialogWindow;
        dialog->setContent(
            tr("程设"),
            tr("欢迎来到异世界。我是程序设计——你接下来一学期的同伴。\n"
               "（此处为占位剧情，成员 A 将通过 StoryEngine 加载真实剧本）"),
            {tr("……你好"), tr("我要回去！")}
        );

        connect(dialog, &DialogWindow::choiceMade, this, [](int idx) {
            Q_UNUSED(idx);
            GameManager::instance().requestScene(GameScene::Map);
        });

        layout->addWidget(dialog);
        layout->setContentsMargins(60, 40, 60, 40);
    }
    stack_->addWidget(dialogPage);

    // 页面 3：小游戏（演示井字棋）
    auto* miniGamePage = new QWidget;
    {
        auto* layout = new QVBoxLayout(miniGamePage);
        auto* game = new TicTacToeGame;
        auto* backBtn = new QPushButton(tr("← 放弃返回"));

        // 监听小游戏结束信号，转发到 GameManager
        connect(game, &MiniGame::finished, this, [](int score, bool won) {
            GameManager::instance().onMiniGameFinished(MiniGameType::TicTacToe, score, won);
        });
        connect(backBtn, &QPushButton::clicked, this, []() {
            GameManager::instance().requestScene(GameScene::Map);
        });

        game->start();
        layout->addWidget(game);
        layout->addWidget(backBtn);
        layout->setContentsMargins(60, 20, 60, 20);
    }
    stack_->addWidget(miniGamePage);

    // 页面 4：结局（占位）
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

    mainLayout->addWidget(stack_, 1);
    setCentralWidget(central);
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
