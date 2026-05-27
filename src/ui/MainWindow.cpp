#include "MainWindow.h"
#include "MainMenu.h"
#include "StatusBar.h"
#include "core/GameManager.h"
#include "core/Player.h"
#include "core/SaveManager.h"
#include "story/DialogWindow.h"
#include "story/StoryEngine.h"
#include "map/MapScene.h"
#include "games/TicTacToeGame.h"
#include "games/BlackjackGame.h"
#include "games/MinesweeperGame.h"
#include "games/MemoryGame.h"
#include "games/MazeGame.h"

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
    connect(mainMenu,
            &MainMenu::loadGameClicked,
            this,
            [this]() {

                bool ok =
                    SaveManager::load(
                        GameManager::instance().player(),
                        1
                        );

                if(ok) {

                    QMessageBox::information(
                        this,
                        tr("读取成功"),
                        tr("已读取存档 1")
                        );

                    GameManager::instance()
                        .requestScene(GameScene::Map);

                } else {

                    QMessageBox::warning(
                        this,
                        tr("读取失败"),
                        tr("未找到存档")
                        );
                }
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
        auto* saveBtn =
            new QPushButton(tr("保存游戏"));
        connect(saveBtn,
                &QPushButton::clicked,
                this,
                [this]() {

                    bool ok =
                        SaveManager::save(
                            GameManager::instance().player(),
                            1
                            );

                    if(ok) {

                        QMessageBox::information(
                            this,
                            tr("保存成功"),
                            tr("已保存到存档 1")
                            );

                    } else {

                        QMessageBox::warning(
                            this,
                            tr("保存失败"),
                            tr("无法保存游戏")
                            );
                    }
                });
        layout->addWidget(mapView, 1);
        layout->addWidget(saveBtn);
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

    // ========== 页面 3：小游戏==========

    auto* blackjackGame = new BlackjackGame;
    auto* ticGame = new TicTacToeGame;
    auto* mineGame = new MinesweeperGame;
    auto* mazeGame = new MazeGame;
    auto* memoryGame = new MemoryGame;
    auto* miniGameMenuPage = new QWidget;

    {
        auto* layout = new QVBoxLayout(miniGameMenuPage);

        auto* title = new QLabel(tr("小游戏中心"));
        title->setAlignment(Qt::AlignCenter);

        title->setStyleSheet(
            "font-size: 28px;"
            "font-weight: bold;"
            "color: #8B1A1A;"
            );

        layout->addWidget(title);

                // =========================
                // 六个小游戏按钮
                // =========================

        auto* blackjackBtn =
            new QPushButton(tr("21点"));

        auto* ticBtn =
            new QPushButton(tr("井字棋"));

        auto* mineBtn =
            new QPushButton(tr("扫雷"));
        auto* memoryBtn =
            new QPushButton(tr("记忆翻牌"));


        auto* mazeBtn =
            new QPushButton(tr("AI迷宫"));

                // 按钮样式
        QList<QPushButton*> btns = {
            blackjackBtn,
            ticBtn,
            mineBtn,
            memoryBtn,
            mazeBtn
        };

        for(auto* b : btns) {

            b->setMinimumHeight(60);

            b->setStyleSheet(
                "font-size:20px;"
                "background:#F5E6CA;"
                "border-radius:10px;"
                );

            layout->addWidget(b);
        }

        layout->addStretch();

        auto* backBtn =
            new QPushButton(tr("← 返回地图"));

        connect(backBtn,
                &QPushButton::clicked,this, [](){
                    GameManager::instance()
                    .requestScene(GameScene::Map);
                });

        layout->addWidget(backBtn);

                // =========================
                // 按钮进入不同小游戏
                // =========================

        connect(blackjackBtn,
                &QPushButton::clicked,
                this,
                [this, blackjackGame]() {

                    blackjackGame->start();

                    stack_->setCurrentIndex(blackjackIndex_);
                });

        connect(ticBtn,
                &QPushButton::clicked,
                this,
                [this, ticGame]() {

                    ticGame->start();

                    stack_->setCurrentIndex(ticTacToeIndex_);
                });

        connect(mineBtn,
                &QPushButton::clicked,
                this,
                [this, mineGame]() {

                    mineGame->start();

                    stack_->setCurrentIndex(minesweeperIndex_);
                });
        connect(memoryBtn,
                &QPushButton::clicked,
                this,
                [this, memoryGame]() {

                    memoryGame->start();

                    stack_->setCurrentIndex(memoryIndex_);
                });

        connect(mazeBtn,
                &QPushButton::clicked,
                this,
                [this, mazeGame]() {

                    mazeGame->start();

                    stack_->setCurrentIndex(mazeIndex_);
                });

    }
    miniGameIndex_ = stack_->addWidget(miniGameMenuPage);

    //21点
    auto* blackjackPage = new QWidget;


    {
        auto* layout =
            new QVBoxLayout(blackjackPage);

        auto* backBtn =
            new QPushButton(tr("← 返回大厅"));

        connect(blackjackGame,
                &MiniGame::finished,
                this,
                [](int score, bool won){

                    GameManager::instance()
                    .onMiniGameFinished(
                        MiniGameType::Blackjack,
                        score,
                        won
                        );
                });

        connect(backBtn,
                &QPushButton::clicked,
                this,
                [this](){

                    stack_->setCurrentIndex(3);
                });



        layout->addWidget(blackjackGame);

        layout->addWidget(backBtn);
    }

    blackjackIndex_ =stack_->addWidget(blackjackPage);
    //井字棋
    auto* ticPage = new QWidget;

    {
        auto* layout =
            new QVBoxLayout(ticPage);

        auto* game = ticGame;

        auto* backBtn =
            new QPushButton(tr("← 返回大厅"));

        connect(game,
                &MiniGame::finished,
                this,
                [](int score, bool won){

                    GameManager::instance()
                    .onMiniGameFinished(
                        MiniGameType::TicTacToe,
                        score,
                        won
                        );
                });

        connect(backBtn,
                &QPushButton::clicked,
                this,
                [this](){

                    stack_->setCurrentIndex(miniGameIndex_);
                });



        layout->addWidget(game);

        layout->addWidget(backBtn);
    }

    ticTacToeIndex_ =stack_->addWidget(ticPage);
    //扫雷
    auto* minePage = new QWidget;

    {
        auto* layout =
            new QVBoxLayout(minePage);

        auto* game = mineGame;

        auto* backBtn =
            new QPushButton(tr("← 返回大厅"));

        connect(game,
                &MiniGame::finished,
                this,
                [](int score, bool won){

                    GameManager::instance()
                    .onMiniGameFinished(
                        MiniGameType::Minesweeper,
                        score,
                        won
                        );
                });

        connect(backBtn,
                &QPushButton::clicked,
                this,
                [this](){

                    stack_->setCurrentIndex(miniGameIndex_);
                });


        layout->addWidget(game);

        layout->addWidget(backBtn);
    }

    minesweeperIndex_ =stack_->addWidget(minePage);

    //记忆翻牌
    auto* memoryPage = new QWidget;

    {
        auto* layout =
            new QVBoxLayout(memoryPage);

        auto* game = memoryGame;

        auto* backBtn =
            new QPushButton(tr("← 返回大厅"));

        connect(game,
                &MiniGame::finished,
                this,
                [](int score, bool won){

                    GameManager::instance()
                    .onMiniGameFinished(
                        MiniGameType::MemoryMatch,
                        score,
                        won
                        );
                });

        connect(backBtn,
                &QPushButton::clicked,
                this,
                [this](){

                    stack_->setCurrentIndex(miniGameIndex_);
                });

        layout->addWidget(game);

        layout->addWidget(backBtn);
    }

    memoryIndex_ = stack_->addWidget(memoryPage);

    auto* mazePage = new QWidget;

    {
        auto* layout =
            new QVBoxLayout(mazePage);

        auto* game = mazeGame;

        auto* backBtn =
            new QPushButton(tr("← 返回大厅"));

        connect(game,
                &MiniGame::finished,
                this,
                [](int score, bool won){

                    GameManager::instance()
                    .onMiniGameFinished(
                        MiniGameType::Maze,
                        score,
                        won
                        );
                });

        connect(backBtn,
                &QPushButton::clicked,
                this,
                [this](){

                    stack_->setCurrentIndex(miniGameIndex_);
                });



        layout->addWidget(game);

        layout->addWidget(backBtn);
    }

    mazeIndex_ = stack_->addWidget(mazePage);

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
