#include "MainWindow.h"
#include "MainMenu.h"
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
#include "common/Subject.h"

#include <QStackedWidget>
#include <QGraphicsView>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QWidget>
#include <QApplication>
#include <QMessageBox>
#include <QInputDialog>
#include <QDialog>
#include <QFormLayout>
#include <QSpinBox>
#include <QGroupBox>
#include <QScrollArea>

namespace SA {

// ── 小工具：创建带颜色进度条 ──────────────────────────────
static QProgressBar* makeBar(const QString& color, QWidget* parent) {
    auto* bar = new QProgressBar(parent);
    bar->setRange(0, 100);
    bar->setTextVisible(true);
    bar->setMaximumHeight(22);
    bar->setStyleSheet(QString(
                           "QProgressBar { border:1px solid #ccc; border-radius:4px; text-align:center; }"
                           "QProgressBar::chunk { background:%1; border-radius:4px; }").arg(color));
    return bar;
}

// ════════════════════════════════════════════════════════════
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      stack_(new QStackedWidget(this)),
      storyEngine_(nullptr),
      dialogWindow_(nullptr)
{
    setWindowTitle(tr("学海漫游：异世界的信科少女"));
    resize(1100, 800);

    storyEngine_ = new StoryEngine(GameManager::instance().player(), this);

    setupUi();
    connectSignals();

    stack_->setCurrentIndex(mainMenuIndex_);
}

// ════════════════════════════════════════════════════════════
void MainWindow::setupUi() {
    // 直接用 stack_ 作为中央控件（不再有顶部状态栏）
    setCentralWidget(stack_);

            // ── 页面 0：主菜单 ────────────────────────────────────
    auto* mainMenu = new MainMenu;
    connect(mainMenu, &MainMenu::startNewGameClicked, this, [this]() {
        GameManager::instance().player()->reset();
        loadAndShowScript(":/scripts/week1.json");
    });
    connect(mainMenu, &MainMenu::loadGameClicked, this, [this]() {
        bool ok = SaveManager::load(GameManager::instance().player(), 1);
        if (ok) {
            QMessageBox::information(this, tr("读取成功"), tr("已读取存档 1"));
            GameManager::instance().requestScene(GameScene::Map);
        } else {
            QMessageBox::warning(this, tr("读取失败"), tr("未找到存档"));
        }
    });
    connect(mainMenu, &MainMenu::settingsClicked, this, [this]() {
        // 进入设置页
        stack_->setCurrentIndex(settingsIndex_);
    });
    connect(mainMenu, &MainMenu::exitClicked, this, []() {
        QApplication::quit();
    });
    stack_->addWidget(mainMenu);          // index 0

            // ── 页面 1：地图 ──────────────────────────────────────
    auto* mapPage = new QWidget;
    {
        auto* layout   = new QVBoxLayout(mapPage);
        auto* mapScene = new MapScene(this);
        auto* mapView  = new QGraphicsView(mapScene);
        mapView->setRenderHint(QPainter::Antialiasing);
        mapView->setMinimumHeight(500);

        connect(mapScene, &MapScene::locationClicked, this, [this](Location loc) {
            auto* player = GameManager::instance().player();
            switch (loc) {
                case Location::Classroom:
                    loadAndShowScript(":/scripts/week1.json");
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

        auto* btnRow  = new QHBoxLayout;
        auto* backBtn = new QPushButton(tr("← 主菜单"));
        auto* saveBtn = new QPushButton(tr("💾 保存游戏"));
        auto* setBtn  = new QPushButton(tr("⚙ 设置"));

        for (auto* b : {backBtn, saveBtn, setBtn}) {
            b->setMinimumHeight(36);
            b->setStyleSheet(
                "QPushButton { font-size:14px; border-radius:6px;"
                "  background:#F5E6CA; color:#4A0000; }"
                "QPushButton:hover { background:#e8d0a0; }");
        }

        connect(backBtn, &QPushButton::clicked, this, []() {
            GameManager::instance().requestScene(GameScene::MainMenu);
        });
        connect(saveBtn, &QPushButton::clicked, this, [this]() {
            bool ok = SaveManager::save(GameManager::instance().player(), 1);
            if (ok)
                QMessageBox::information(this, tr("保存成功"), tr("已保存到存档 1"));
            else
                QMessageBox::warning(this, tr("保存失败"), tr("无法保存游戏"));
        });
        connect(setBtn, &QPushButton::clicked, this, [this]() {
            stack_->setCurrentIndex(settingsIndex_);
        });

        btnRow->addWidget(backBtn);
        btnRow->addStretch();
        btnRow->addWidget(saveBtn);
        btnRow->addWidget(setBtn);

        layout->addWidget(mapView, 1);
        layout->addLayout(btnRow);
        layout->setContentsMargins(20, 10, 20, 10);
    }
    stack_->addWidget(mapPage);           // index 1

            // ── 页面 2：对话 ──────────────────────────────────────
    auto* dialogPage = new QWidget;
    {
        auto* layout  = new QVBoxLayout(dialogPage);
        dialogWindow_ = new DialogWindow;

        connect(dialogWindow_, &DialogWindow::choiceMade, this, [this](int idx) {
            storyEngine_->onChoiceSelected(idx);
        });
        connect(storyEngine_, &StoryEngine::nodeChanged, this, [this]() {
            refreshDialogFromEngine();
        });
        connect(storyEngine_, &StoryEngine::scriptFinished, this, [this]() {
            QMessageBox::information(this, tr("剧情结束"), tr("本段剧情已结束。"));
            GameManager::instance().requestScene(GameScene::Map);
        });

        layout->addWidget(dialogWindow_);
        layout->setContentsMargins(0, 0, 0, 0);
    }
    stack_->addWidget(dialogPage);        // index 2

            // ── 页面 3：小游戏大厅 ───────────────────────────────
    auto* blackjackGame = new BlackjackGame;
    auto* ticGame       = new TicTacToeGame;
    auto* mineGame      = new MinesweeperGame;
    auto* mazeGame      = new MazeGame;
    auto* memoryGame    = new MemoryGame;
    auto* miniGameMenuPage = new QWidget;
    {
        auto* layout = new QVBoxLayout(miniGameMenuPage);
        auto* title  = new QLabel(tr("小游戏中心"));
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet("font-size:28px; font-weight:bold; color:#8B1A1A;");
        layout->addWidget(title);

        auto* blackjackBtn = new QPushButton(tr("21点"));
        auto* ticBtn       = new QPushButton(tr("井字棋"));
        auto* mineBtn      = new QPushButton(tr("扫雷"));
        auto* memoryBtn    = new QPushButton(tr("记忆翻牌"));
        auto* mazeBtn      = new QPushButton(tr("AI迷宫"));

        QString btnStyle =
            "QPushButton { font-size:20px; background:#F5E6CA;"
            "  border-radius:10px; color:#4A0000; }"
            "QPushButton:hover { background:#e8d0a0; }";

        for (auto* b : {blackjackBtn, ticBtn, mineBtn, memoryBtn, mazeBtn}) {
            b->setMinimumHeight(60);
            b->setStyleSheet(btnStyle);
            layout->addWidget(b);
        }
        layout->addStretch();

        auto* backBtn = new QPushButton(tr("← 返回地图"));
        backBtn->setStyleSheet(btnStyle);
        connect(backBtn, &QPushButton::clicked, this, []() {
            GameManager::instance().requestScene(GameScene::Map);
        });
        layout->addWidget(backBtn);

        connect(blackjackBtn, &QPushButton::clicked, this, [this, blackjackGame]() {
            blackjackGame->start(); stack_->setCurrentIndex(blackjackIndex_);
        });
        connect(ticBtn, &QPushButton::clicked, this, [this, ticGame]() {
            ticGame->start(); stack_->setCurrentIndex(ticTacToeIndex_);
        });
        connect(mineBtn, &QPushButton::clicked, this, [this, mineGame]() {
            mineGame->start(); stack_->setCurrentIndex(minesweeperIndex_);
        });
        connect(memoryBtn, &QPushButton::clicked, this, [this, memoryGame]() {
            memoryGame->start(); stack_->setCurrentIndex(memoryIndex_);
        });
        connect(mazeBtn, &QPushButton::clicked, this, [this, mazeGame]() {
            mazeGame->start(); stack_->setCurrentIndex(mazeIndex_);
        });
    }
    miniGameIndex_ = stack_->addWidget(miniGameMenuPage); // index 3 (endingIndex_ 需要调整)

            // ── 小游戏各页 ────────────────────────────────────────
    auto makeMiniPage = [&](QWidget* game, MiniGameType type, int& outIndex) {
        auto* page   = new QWidget;
        auto* layout = new QVBoxLayout(page);
        auto* back   = new QPushButton(tr("← 返回大厅"));
        back->setStyleSheet(
            "QPushButton { font-size:14px; background:#F5E6CA;"
            "  border-radius:6px; color:#4A0000; }"
            "QPushButton:hover { background:#e8d0a0; }");
        if (auto* miniGame = qobject_cast<SA::MiniGame*>(game)) {
            connect(miniGame, &SA::MiniGame::finished, this, [type](int score, bool won) {
                GameManager::instance().onMiniGameFinished(type, score, won);
            });
        }
        connect(back, &QPushButton::clicked, this, [this]() {
            stack_->setCurrentIndex(miniGameIndex_);
        });
        layout->addWidget(game);
        layout->addWidget(back);
        outIndex = stack_->addWidget(page);
    };

    makeMiniPage(blackjackGame, MiniGameType::Blackjack,    blackjackIndex_);
    makeMiniPage(ticGame,       MiniGameType::TicTacToe,    ticTacToeIndex_);
    makeMiniPage(mineGame,      MiniGameType::Minesweeper,  minesweeperIndex_);
    makeMiniPage(memoryGame,    MiniGameType::MemoryMatch,  memoryIndex_);
    makeMiniPage(mazeGame,      MiniGameType::Maze,         mazeIndex_);

            // ── 页面：结局 ────────────────────────────────────────
    auto* endingPage = new QWidget;
    {
        auto* layout = new QVBoxLayout(endingPage);
        auto* label  = new QLabel(tr("[ 结局演出 ]\n\n（由 EndingJudge 判定后展示对应结局）"));
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-size:28px; color:#8B1A1A;");
        auto* backBtn = new QPushButton(tr("← 返回主菜单"));
        backBtn->setStyleSheet(
            "QPushButton { font-size:16px; background:#F5E6CA; border-radius:8px; }"
            "QPushButton:hover { background:#e8d0a0; }");
        connect(backBtn, &QPushButton::clicked, this, []() {
            GameManager::instance().requestScene(GameScene::MainMenu);
        });
        layout->addStretch();
        layout->addWidget(label);
        layout->addStretch();
        layout->addWidget(backBtn);
        layout->setContentsMargins(60, 60, 60, 60);
    }
    endingIndex_ = stack_->addWidget(endingPage);

            // ── 页面：设置 ────────────────────────────────────────
    auto* settingsPage = new QWidget;
    settingsPage->setStyleSheet("background: #FFF8EE;");
    {
        auto* outerLayout = new QVBoxLayout(settingsPage);
        outerLayout->setContentsMargins(40, 30, 40, 30);
        outerLayout->setSpacing(20);

                // 标题
        auto* titleLabel = new QLabel(tr("设  置"));
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet(
            "font-size:28px; font-weight:bold; color:#8B1A1A;");
        outerLayout->addWidget(titleLabel);

                // ── 属性面板 ─────────────────────────────────────
        auto* attrGroup = new QGroupBox(tr("角色属性"));
        attrGroup->setStyleSheet(
            "QGroupBox { font-size:16px; font-weight:bold; color:#4A0000;"
            "  border:2px solid #C8A070; border-radius:8px; margin-top:8px; }"
            "QGroupBox::title { subcontrol-origin:margin; padding:0 6px; }");

        auto* attrGrid = new QGridLayout(attrGroup);
        attrGrid->setSpacing(10);

        auto* player = GameManager::instance().player();

                // 日期
        auto* dateLabel = new QLabel(
            tr("当前进度：第 %1 周·第 %2 天")
                .arg(player->currentWeek()).arg(player->currentDay()));
        dateLabel->setStyleSheet("font-size:14px; color:#333;");
        attrGrid->addWidget(dateLabel, 0, 0, 1, 4);

                // 好感度进度条
        struct BarInfo { SubjectType type; QString name; QString color; };
        QList<BarInfo> barInfos = {
                                   { SubjectType::ProgDesign,     tr("程序设计好感"), "#E91E8C" },
                                   { SubjectType::Calculus,       tr("高数好感"),     "#2196F3" },
                                   { SubjectType::LinearAlgebra,  tr("线代好感"),     "#9C27B0" },
                                   { SubjectType::AIIntro,        tr("AI导论好感"),   "#FF9800" },
                                   };

        int row = 1;
        for (auto& info : barInfos) {
            auto* lbl = new QLabel(info.name + ":", attrGroup);
            lbl->setStyleSheet("font-size:14px;");
            auto* bar = makeBar(info.color, attrGroup);
            bar->setValue(player->affinity(info.type));

                    // 连接 player 信号实时刷新
            connect(player, &Player::affinityChanged,
                    bar, [bar, t = info.type](SubjectType s, int v) {
                        if (s == t) bar->setValue(v);
                    });

            attrGrid->addWidget(lbl, row, 0);
            attrGrid->addWidget(bar, row, 1, 1, 3);
            row++;
        }

                // 压力 & 黑化
        auto* stressLbl = new QLabel(tr("压力值:"), attrGroup);
        stressLbl->setStyleSheet("font-size:14px;");
        auto* stressBar = makeBar("#3B5BA5", attrGroup);
        stressBar->setValue(player->stress());
        connect(player, &Player::stressChanged, stressBar, &QProgressBar::setValue);

        auto* darkLbl = new QLabel(tr("黑化值:"), attrGroup);
        darkLbl->setStyleSheet("font-size:14px;");
        auto* darkBar = makeBar("#5A189A", attrGroup);
        darkBar->setValue(player->darkness());
        connect(player, &Player::darknessChanged, darkBar, &QProgressBar::setValue);

        attrGrid->addWidget(stressLbl, row,   0);
        attrGrid->addWidget(stressBar, row,   1, 1, 3);
        attrGrid->addWidget(darkLbl,   row+1, 0);
        attrGrid->addWidget(darkBar,   row+1, 1, 1, 3);

                // 日期实时刷新
        connect(player, &Player::dateChanged, dateLabel,
                [dateLabel](int w, int d) {
                    dateLabel->setText(
                        tr("当前进度：第 %1 周·第 %2 天").arg(w).arg(d));
                });

        outerLayout->addWidget(attrGroup);

                // ── 存档/读档面板 ─────────────────────────────────
        auto* saveGroup = new QGroupBox(tr("存档管理"));
        saveGroup->setStyleSheet(
            "QGroupBox { font-size:16px; font-weight:bold; color:#4A0000;"
            "  border:2px solid #C8A070; border-radius:8px; margin-top:8px; }"
            "QGroupBox::title { subcontrol-origin:margin; padding:0 6px; }");

        auto* saveLayout = new QHBoxLayout(saveGroup);
        saveLayout->setSpacing(20);

        auto* saveBtn = new QPushButton(tr("💾  保存到存档 1"));
        auto* loadBtn = new QPushButton(tr("📂  读取存档 1"));

        QString saveBtnStyle =
            "QPushButton { font-size:16px; background:#8B1A1A; color:white;"
            "  border-radius:8px; padding:10px 24px; }"
            "QPushButton:hover { background:#B22222; }";
        QString loadBtnStyle =
            "QPushButton { font-size:16px; background:#3B5BA5; color:white;"
            "  border-radius:8px; padding:10px 24px; }"
            "QPushButton:hover { background:#5070C0; }";

        saveBtn->setStyleSheet(saveBtnStyle);
        loadBtn->setStyleSheet(loadBtnStyle);
        saveBtn->setMinimumHeight(50);
        loadBtn->setMinimumHeight(50);

        connect(saveBtn, &QPushButton::clicked, this, [this]() {
            bool ok = SaveManager::save(GameManager::instance().player(), 1);
            if (ok)
                QMessageBox::information(this, tr("保存成功"), tr("已保存到存档 1"));
            else
                QMessageBox::warning(this, tr("保存失败"), tr("无法保存游戏"));
        });
        connect(loadBtn, &QPushButton::clicked, this, [this]() {
            bool ok = SaveManager::load(GameManager::instance().player(), 1);
            if (ok) {
                QMessageBox::information(this, tr("读取成功"), tr("已读取存档 1"));
                GameManager::instance().requestScene(GameScene::Map);
            } else {
                QMessageBox::warning(this, tr("读取失败"), tr("未找到存档"));
            }
        });

        saveLayout->addStretch();
        saveLayout->addWidget(saveBtn);
        saveLayout->addWidget(loadBtn);
        saveLayout->addStretch();

        outerLayout->addWidget(saveGroup);
        outerLayout->addStretch();

                // ── 返回按钮 ──────────────────────────────────────
        auto* backBtn = new QPushButton(tr("← 返回"));
        backBtn->setMinimumHeight(44);
        backBtn->setStyleSheet(
            "QPushButton { font-size:16px; background:#F5E6CA; color:#4A0000;"
            "  border-radius:8px; }"
            "QPushButton:hover { background:#e8d0a0; }");
        // 返回时回到来源页（主菜单或地图，用 stack 当前index判断不可靠）
        // 简单方案：返回到地图，如果玩家未开始游戏则回主菜单
        connect(backBtn, &QPushButton::clicked, this, [this]() {
            // 如果之前来自主菜单（week==1 day==1 且没有存档变化），回主菜单
            // 简单起见：直接退回上一个 index（用 QStackedWidget 记录）
            auto* player = GameManager::instance().player();
            if (player->currentWeek() == 1 && player->currentDay() == 1) {
                stack_->setCurrentIndex(mainMenuIndex_);
            } else {
                stack_->setCurrentIndex(mapIndex_);
            }
        });
        outerLayout->addWidget(backBtn);
    }
    settingsIndex_ = stack_->addWidget(settingsPage);
}

// ════════════════════════════════════════════════════════════
void MainWindow::connectSignals() {
    connect(&GameManager::instance(), &GameManager::sceneChangeRequested,
            this, &MainWindow::onSceneChangeRequested);
}

void MainWindow::onSceneChangeRequested(GameScene scene) {
    switch (scene) {
        case GameScene::MainMenu: stack_->setCurrentIndex(mainMenuIndex_);  break;
        case GameScene::Map:      stack_->setCurrentIndex(mapIndex_);       break;
        case GameScene::Dialog:   stack_->setCurrentIndex(dialogIndex_);    break;
        case GameScene::MiniGame: stack_->setCurrentIndex(miniGameIndex_);  break;
        case GameScene::Ending:   stack_->setCurrentIndex(endingIndex_);    break;
    }
}

void MainWindow::loadAndShowScript(const QString& scriptPath) {
    if (storyEngine_->loadScript(scriptPath)) {
        GameManager::instance().requestScene(GameScene::Dialog);
    } else {
        QMessageBox::warning(this, tr("错误"),
                             tr("无法加载剧本：%1").arg(scriptPath));
    }
}

void MainWindow::refreshDialogFromEngine() {
    if (!storyEngine_ || !dialogWindow_) return;

    QString speaker    = storyEngine_->currentSpeaker();
    QString text       = storyEngine_->currentText();
    QStringList choices = storyEngine_->currentChoices();

    if (choices.isEmpty())
        choices.append(tr("（继续）"));

    dialogWindow_->setContent(speaker, text, choices);
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_F12) {
        showDebugMenu();
        return;
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::showDebugMenu() {
    auto* player = GameManager::instance().player();
    QDialog dialog(this);
    dialog.setWindowTitle("调试菜单");
    QFormLayout layout(&dialog);

    QSpinBox progBox, calBox, linearBox, aiBox, stressBox, darkBox;
    for (auto* b : {&progBox, &calBox, &linearBox, &aiBox, &stressBox, &darkBox})
        b->setRange(0, 100);

    progBox.setValue(player->affinity(SubjectType::ProgDesign));
    calBox.setValue(player->affinity(SubjectType::Calculus));
    linearBox.setValue(player->affinity(SubjectType::LinearAlgebra));
    aiBox.setValue(player->affinity(SubjectType::AIIntro));
    stressBox.setValue(player->stress());
    darkBox.setValue(player->darkness());

    layout.addRow("程序设计好感", &progBox);
    layout.addRow("高数好感",     &calBox);
    layout.addRow("线代好感",     &linearBox);
    layout.addRow("AI导论好感",   &aiBox);
    layout.addRow("压力值",       &stressBox);
    layout.addRow("线代黑化值",   &darkBox);

    QPushButton applyBtn("应用"), cancelBtn("取消");
    layout.addRow(&applyBtn, &cancelBtn);

    connect(&cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
    connect(&applyBtn,  &QPushButton::clicked, [&]() {
        player->setAffinity(SubjectType::ProgDesign,    progBox.value());
        player->setAffinity(SubjectType::Calculus,      calBox.value());
        player->setAffinity(SubjectType::LinearAlgebra, linearBox.value());
        player->setAffinity(SubjectType::AIIntro,       aiBox.value());
        player->setStress(stressBox.value());
        player->setDarkness(darkBox.value());
        dialog.accept();
    });

    dialog.exec();
}

} // namespace SA