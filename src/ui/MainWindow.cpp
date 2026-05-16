#include "MainWindow.h"
#include "MainMenu.h"
#include "StatusBar.h"
#include "core/GameManager.h"
#include "core/Player.h"
#include "story/DialogWindow.h"
#include "story/StoryEngine.h"
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
      stack_(new QStackedWidget(this)),
      storyEngine_(nullptr),
      dialogWindow_(nullptr)
{
    setWindowTitle(tr("学海漫游：异世界的信科少女"));
    resize(1100, 800);

    // 创建 StoryEngine（成员 A 的核心：连接剧本和 UI）
    storyEngine_ = new StoryEngine(GameManager::instance().player(), this);

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
    connect(mainMenu, &MainMenu::startNewGameClicked, this, [this]() {
        // 开始游戏：重置玩家数据 + 加载第一周剧本 + 切到对话场景
        GameManager::instance().player()->reset();
        loadAndShowScript(":/scripts/week1.json");
    });
    connect(mainMenu, &MainMenu::loadGameClicked, this, [this]() {
        QMessageBox::information(this, tr("提示"),
            tr("读档功能由成员 C 实现，详见 src/core/SaveManager.cpp"));
    });
    connect(mainMenu, &MainMenu::settingsClicked, this, [this]() {
        QMessageBox::information(this, tr("提示"),
            tr("设置面板由成员 B 实现"));
    });
    connect(mainMenu, &MainMenu::exitClicked, this, []() {
        QApplication::quit();
    });
    stack_->addWidget(mainMenu);

    // ========== 页面 1：地图 ==========
    auto* mapPage = new QWidget;
    {
        auto* layout = new QVBoxLayout(mapPage);
        auto* mapScene = new MapScene(this);
        auto* mapView = new QGraphicsView(mapScene);
        mapView->setRenderHint(QPainter::Antialiasing);
        mapView->setMinimumHeight(500);

        connect(mapScene, &MapScene::locationClicked, this, [this](Location loc) {
            auto* player = GameManager::instance().player();
            switch (loc) {
                case Location::Classroom:
                    // 进入教学楼 = 触发当前周的剧本
                    // 简单实现：始终加载 week1；真实版本应根据 player->currentWeek() 选剧本
                    loadAndShowScript(":/scripts/week1.json");
                    break;
                case Location::Library:
                    GameManager::instance().requestScene(GameScene::MiniGame);
                    break;
                case Location::WeimingLake:
                    // 减压剧情（简化版）
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

    // ========== 页面 2：对话（接入 StoryEngine！）==========
    auto* dialogPage = new QWidget;
    {
        auto* layout = new QVBoxLayout(dialogPage);
        dialogWindow_ = new DialogWindow;

        // 玩家选择选项 → 通知 StoryEngine 推进剧情
        connect(dialogWindow_, &DialogWindow::choiceMade, this, [this](int idx) {
            storyEngine_->onChoiceSelected(idx);
        });

        // StoryEngine 切换节点 → 刷新对话框显示
        connect(storyEngine_, &StoryEngine::nodeChanged, this, [this]() {
            refreshDialogFromEngine();
        });

        // StoryEngine 剧本结束 → 返回地图
        connect(storyEngine_, &StoryEngine::scriptFinished, this, [this]() {
            QMessageBox::information(this, tr("剧情结束"),
                tr("本段剧情已结束。"));
            GameManager::instance().requestScene(GameScene::Map);
        });

        layout->addWidget(dialogWindow_);
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

// ============================================================
// 加载剧本并切到对话场景
// ============================================================
void MainWindow::loadAndShowScript(const QString& scriptPath) {
    if (storyEngine_->loadScript(scriptPath)) {
        GameManager::instance().requestScene(GameScene::Dialog);
    } else {
        QMessageBox::warning(this, tr("错误"),
            tr("无法加载剧本：%1").arg(scriptPath));
    }
}

// ============================================================
// 从 StoryEngine 取当前节点信息，刷新到 DialogWindow
// ============================================================
void MainWindow::refreshDialogFromEngine() {
    if (!storyEngine_ || !dialogWindow_) return;

    QString speaker = storyEngine_->currentSpeaker();
    QString text = storyEngine_->currentText();
    QStringList choices = storyEngine_->currentChoices();

    // 如果没有选项（如旁白或叙述节点），加一个"继续"按钮
    if (choices.isEmpty()) {
        choices.append(tr("（继续）"));
    }

    dialogWindow_->setContent(speaker, text, choices);
}

} // namespace SA
