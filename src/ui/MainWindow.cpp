#include "MainWindow.h"
#include "MainMenu.h"
#include "StatusBar.h"
#include "core/GameManager.h"
#include "core/Player.h"
#include "story/DialogWindow.h"
#include "map/MapScene.h"
#include "games/TicTacToeGame.h"

#include <QStackedWidget>
#include <QGraphicsView>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QApplication>
#include <QMessageBox>

namespace SA {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      stack_(new QStackedWidget(this))
{
    setWindowTitle(tr("学海漫游：异世界的信科少女"));
    resize(1100, 800);

    setupUi();
    connectSignals();

    stack_->setCurrentIndex(mainMenuIndex_);
}

void MainWindow::setupUi() {
    auto* central = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // ========== 顶部状态栏 ==========
    auto* statusBar = new StatusBar(GameManager::instance().player(), this);
    mainLayout->addWidget(statusBar);

    // ========== 页面 0：主菜单 ==========
    auto* mainMenu = new MainMenu;
    connect(mainMenu, &MainMenu::startNewGameClicked, this, []() {
        GameManager::instance().requestScene(GameScene::Map);
    });
    connect(mainMenu, &MainMenu::loadGameClicked, this, [this]() {
        QMessageBox::information(this, tr("提示"),
            tr("读档功能由成员 C 实现，详见 src/core/SaveManager.cpp"));
    });
    connect(mainMenu, &MainMenu::settingsClicked, this, [this]() {
        SettingsDialog dialog(this);
        dialog.exec();
    });
    connect(mainMenu, &MainMenu::exitClicked, this, []() {
        QApplication::quit();
    });
    stack_->addWidget(mainMenu);

    // ========== 页面 1：地图（真实 MapScene）==========
    auto* mapPage = new QWidget;
    {
        auto* layout = new QVBoxLayout(mapPage);
        auto* mapScene = new MapScene(this);
        auto* mapView = new QGraphicsView(mapScene);
        mapView->setRenderHint(QPainter::Antialiasing);
        mapView->setMinimumHeight(500);

        connect(mapScene, &MapScene::locationClicked, this, [this](Location loc) {
            QString msg = tr("点击了：%1").arg(locationName(loc));
            // 示例：到不同地点触发不同效果
            auto* player = GameManager::instance().player();
            switch (loc) {
                case Location::Classroom:
                    player->addAffinity(SubjectType::ProgDesign, 2);
                    GameManager::instance().requestScene(GameScene::Dialog);
                    break;
                case Location::Library:
                    GameManager::instance().requestScene(GameScene::MiniGame);
                    break;
                case Location::WeimingLake:
                    player->addStress(-5);
                    QMessageBox::information(this, tr("未名湖"),
                        tr("你在湖边坐了一会儿，心情舒畅了不少。\n压力 -5"));
                    break;
                case Location::Dormitory:
                    player->addStress(-10);
                    player->advanceDay();
                    QMessageBox::information(this, tr("宿舍"),
                        tr("回到宿舍睡了一觉。\n压力 -10，时间推进一天"));
                    break;
            }
        });

        auto* backBtn = new QPushButton(tr("← 返回主菜单"));
        connect(backBtn, &QPushButton::clicked, this, []() {
            GameManager::instance().requestScene(GameScene::MainMenu);
        });

        layout->addWidget(mapView, 1);
        layout->addWidget(backBtn);
        layout->setContentsMargins(20, 10, 20, 10);
    }
    stack_->addWidget(mapPage);

    // ========== 页面 2：对话 ==========
    auto* dialogPage = new QWidget;
    {
        auto* layout = new QVBoxLayout(dialogPage);
        auto* dialog = new DialogWindow;
        dialog->setContent(
            tr("程设"),
            tr("欢迎来到异世界。我是程序设计——你接下来一学期的同伴。\n"
               "（占位剧情，成员 A 将通过 StoryEngine 加载真实剧本）"),
            {tr("……你好"), tr("我要回去！")}
        );

        connect(dialog, &DialogWindow::choiceMade, this, [](int idx) {
            auto* player = GameManager::instance().player();
            if (idx == 0) {
                player->addAffinity(SubjectType::ProgDesign, 3);
            } else {
                player->addStress(5);
            }
            GameManager::instance().requestScene(GameScene::Map);
        });

        layout->addWidget(dialog);
        layout->setContentsMargins(60, 40, 60, 40);
    }
    stack_->addWidget(dialogPage);

    // ========== 页面 3：小游戏（井字棋演示）==========
    auto* miniGamePage = new QWidget;
    {
        auto* layout = new QVBoxLayout(miniGamePage);
        auto* game = new TicTacToeGame;
        auto* backBtn = new QPushButton(tr("← 放弃返回"));

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

    // ========== 页面 4：结局 ==========
    auto* endingPage = new QWidget;
    {
        auto* layout = new QVBoxLayout(endingPage);
        auto* label = new QLabel(tr("[ 结局演出 ]\n\n（由 EndingJudge 判定后展示对应结局）"));
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-size: 28px; color: #8B1A1A;");

        auto* backBtn = new QPushButton(tr("← 返回主菜单"));
        connect(backBtn, &QPushButton::clicked, this, []() {
            GameManager::instance().requestScene(GameScene::MainMenu);
        });

        layout->addStretch();
        layout->addWidget(label);
        layout->addStretch();
        layout->addWidget(backBtn);
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
