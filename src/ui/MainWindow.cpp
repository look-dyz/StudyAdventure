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
#include <QDialog>
#include <QFormLayout>
#include <QSpinBox>
#include <QGroupBox>
#include <QFrame>

namespace SA {

static QProgressBar* makeBar(const QString& color, QWidget* parent) {
    auto* bar = new QProgressBar(parent);
    bar->setRange(0, 100);
    bar->setTextVisible(true);
    bar->setMaximumHeight(22);
    bar->setStyleSheet(QString(
                           "QProgressBar{border:1px solid #ccc;border-radius:4px;text-align:center;}"
                           "QProgressBar::chunk{background:%1;border-radius:4px;}").arg(color));
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
    setCentralWidget(stack_);

            // ── 页面：主菜单 ──────────────────────────────────────
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
        stack_->setCurrentIndex(settingsIndex_);
    });
    connect(mainMenu, &MainMenu::exitClicked, this, []() {
        QApplication::quit();
    });
    mainMenuIndex_ = stack_->addWidget(mainMenu);      // ★ 动态记录

    // ── 页面：地图 ────────────────────────────────────────
    auto* mapPage = new QWidget;
    {
        auto* layout   = new QVBoxLayout(mapPage);
        auto* mapScene = new MapScene(this);
        auto* mapView  = new QGraphicsView(mapScene);
        mapView->setRenderHint(QPainter::Antialiasing);
        mapView->setMinimumHeight(500);

        auto* infoBar    = new QHBoxLayout;
        mapDateLabel_    = new QLabel(mapPage);
        mapFreeDayLabel_ = new QLabel(mapPage);

        QString infoStyle =
            "font-size:15px;font-weight:bold;color:#4A0000;"
            "background:rgba(255,240,210,200);border-radius:6px;padding:4px 12px;";
        mapDateLabel_->setStyleSheet(infoStyle);
        mapFreeDayLabel_->setStyleSheet(infoStyle);
        updateMapDateDisplay();

        infoBar->addWidget(mapDateLabel_);
        infoBar->addSpacing(16);
        infoBar->addWidget(mapFreeDayLabel_);
        infoBar->addStretch();

        connect(mapScene, &MapScene::locationPreview,
                this, [this](Location loc,
                             const QString& imagePath,
                             const QString& description) {
                    pendingLocation_ = loc;

                            // 背景图
                    QPixmap px(imagePath);
                    if (!px.isNull()) {
                        locationBgLabel_->setPixmap(px);  // scaleContents=true 自动拉伸
                    } else {
                        locationBgLabel_->clear();
                    }

                            // 从 description 里拆出第一行作为标题，其余作为正文
                            // description 格式：第一行是地点名，空行后是正文
                    QStringList lines = description.split('\n');
                    QString title;
                    QString body;
                    if (!lines.isEmpty()) {
                        title = lines.first();           // 第一行：地点名
                        lines.removeFirst();
                        // 去掉开头的空行
                        while (!lines.isEmpty() && lines.first().trimmed().isEmpty())
                            lines.removeFirst();
                        body = lines.join('\n');
                    }

                            // 找到标题 label 并设置
                    if (auto* tl = locationBgLabel_->parentWidget()
                                       ->findChild<QLabel*>("locationTitleLabel")) {
                        tl->setText(title);
                    }
                    locationDescLabel_->setText(body);

                    stack_->setCurrentIndex(locationPageIndex_);
                });

        auto* btnRow  = new QHBoxLayout;
        auto* backBtn = new QPushButton(tr("← 主菜单"));
        auto* saveBtn = new QPushButton(tr("💾 保存"));
        auto* setBtn  = new QPushButton(tr("⚙ 设置"));
        QString btnStyle =
            "QPushButton{font-size:14px;border-radius:6px;"
            "background:#F5E6CA;color:#4A0000;padding:4px 16px;}"
            "QPushButton:hover{background:#e8d0a0;}";
        for (auto* b : {backBtn, saveBtn, setBtn}) {
            b->setMinimumHeight(36);
            b->setStyleSheet(btnStyle);
        }
        connect(backBtn, &QPushButton::clicked, this, []() {
            GameManager::instance().requestScene(GameScene::MainMenu);
        });
        connect(saveBtn, &QPushButton::clicked, this, [this]() {
            bool ok = SaveManager::save(GameManager::instance().player(), 1);
            QMessageBox::information(this,
                                     ok ? tr("保存成功") : tr("保存失败"),
                                     ok ? tr("已保存到存档 1") : tr("无法保存游戏"));
        });
        connect(setBtn, &QPushButton::clicked, this, [this]() {
            stack_->setCurrentIndex(settingsIndex_);
        });
        btnRow->addWidget(backBtn);
        btnRow->addStretch();
        btnRow->addWidget(saveBtn);
        btnRow->addWidget(setBtn);

        layout->addLayout(infoBar);
        layout->addWidget(mapView, 1);
        layout->addLayout(btnRow);
        layout->setContentsMargins(20, 10, 20, 10);
    }
    mapIndex_ = stack_->addWidget(mapPage);            // ★ 动态记录

    // ── 页面：地点详情 ────────────────────────────────────────
    auto* locationPage = new QWidget;
    locationPage->setStyleSheet("background:black;");
    {
        auto* layout = new QVBoxLayout(locationPage);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

                // 背景图
        // 背景图
        locationBgLabel_ = new QLabel(locationPage);
        locationBgLabel_->setScaledContents(true);
        locationBgLabel_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        locationBgLabel_->setStyleSheet("background:#111;");

                // 底部剧情框容器
        auto* descPanel = new QWidget(locationPage);
        descPanel->setMinimumHeight(200);
        descPanel->setStyleSheet(
            "background: rgba(10, 10, 30, 230);"
            "border-top: 2px solid rgba(200, 170, 100, 200);");

        auto* descLayout = new QVBoxLayout(descPanel);
        descLayout->setContentsMargins(60, 20, 60, 20);
        descLayout->setSpacing(16);

                // 地点名称（大标题）
        auto* locationTitleLabel = new QLabel(descPanel);
        locationTitleLabel->setObjectName("locationTitleLabel");
        locationTitleLabel->setStyleSheet(
            "font-size: 18px;"
            "font-weight: bold;"
            "color: #FFD580;"
            "background: transparent;");

                // 描述文字
        locationDescLabel_ = new QLabel(descPanel);
        locationDescLabel_->setWordWrap(true);
        locationDescLabel_->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        locationDescLabel_->setMinimumHeight(80);
        locationDescLabel_->setStyleSheet(
            "font-size: 14px;"
            "color: #F0E8D0;"
            "background: transparent;"
            "line-height: 180%;");

                // 按钮行
        auto* btnRow     = new QHBoxLayout;
        auto* confirmBtn = new QPushButton(tr("▶  进入"), descPanel);
        auto* cancelBtn  = new QPushButton(tr("✕  返回地图"), descPanel);

        confirmBtn->setMinimumHeight(44);
        cancelBtn->setMinimumHeight(44);
        confirmBtn->setMinimumWidth(140);
        cancelBtn->setMinimumWidth(140);
        confirmBtn->setStyleSheet(
            "QPushButton { font-size:15px; background:#8B1A1A; color:white;"
            "  border-radius:8px; padding:0 32px; }"
            "QPushButton:hover { background:#B22222; }");
        cancelBtn->setStyleSheet(
            "QPushButton { font-size:15px; background:rgba(60,60,80,220);"
            "  color:#DDD; border-radius:8px; padding:0 32px; }"
            "QPushButton:hover { background:rgba(100,100,130,240); }");

        connect(confirmBtn, &QPushButton::clicked, this, [this]() {
            stack_->setCurrentIndex(mapIndex_);
            handleLocationAction(pendingLocation_);
        });
        connect(cancelBtn, &QPushButton::clicked, this, [this]() {
            stack_->setCurrentIndex(mapIndex_);
        });

        btnRow->addStretch();
        btnRow->addWidget(confirmBtn);
        btnRow->addSpacing(24);
        btnRow->addWidget(cancelBtn);
        btnRow->addStretch();

        descLayout->addWidget(locationTitleLabel);
        descLayout->addWidget(locationDescLabel_);
        descLayout->addLayout(btnRow);

        layout->addWidget(locationBgLabel_, 1);
        layout->addWidget(descPanel);
    }
    locationPageIndex_ = stack_->addWidget(locationPage);

            // ── 页面：对话 ────────────────────────────────────────
            // ★ 关键修复：dialogWindow_ 直接填满 dialogPage，背景图才能铺满
    auto* dialogPage = new QWidget;
    {
        // dialogPage 本身用黑色背景兜底
        dialogPage->setStyleSheet("background:black;");

                // dialogWindow_ 直接作为 dialogPage 的唯一子控件，填满整页
        dialogWindow_ = new DialogWindow(dialogPage);

                // 用 QVBoxLayout 让 dialogWindow_ 填满
        auto* layout = new QVBoxLayout(dialogPage);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);

                // ---- 顶部工具栏（悬浮在最上方）----
        auto* topBar = new QWidget(dialogPage);
        topBar->setStyleSheet("background:rgba(0,0,0,140);");
        topBar->setFixedHeight(40);
        auto* topLayout = new QHBoxLayout(topBar);
        topLayout->setContentsMargins(12, 4, 12, 4);

        auto* dialogDateLabel = new QLabel(topBar);
        dialogDateLabel->setStyleSheet("color:white;font-size:13px;");
        auto* player = GameManager::instance().player();
        dialogDateLabel->setText(
            tr("第 %1 周·第 %2 天")
                .arg(player->currentWeek()).arg(player->currentDay()));
        connect(player, &Player::dateChanged, dialogDateLabel,
                [dialogDateLabel](int w, int d) {
                    dialogDateLabel->setText(
                        tr("第 %1 周·第 %2 天").arg(w).arg(d));
                });

        auto* dialogSetBtn = new QPushButton(tr("⚙ 设置"), topBar);
        dialogSetBtn->setStyleSheet(
            "QPushButton{font-size:13px;background:rgba(255,255,255,60);"
            "color:white;border-radius:4px;padding:2px 12px;}"
            "QPushButton:hover{background:rgba(255,255,255,100);}");
        connect(dialogSetBtn, &QPushButton::clicked, this, [this]() {
            stack_->setCurrentIndex(settingsIndex_);
        });

        topLayout->addWidget(dialogDateLabel);
        topLayout->addStretch();
        topLayout->addWidget(dialogSetBtn);

                // 布局：顶栏 + dialogWindow_ 填满剩余空间
        layout->addWidget(topBar);
        layout->addWidget(dialogWindow_, 1);

                // ---- 信号连接 ----
        connect(dialogWindow_, &DialogWindow::choiceMade,
                this, [this](int idx) {
                    storyEngine_->onChoiceSelected(idx);
                });
        connect(storyEngine_, &StoryEngine::nodeChanged,
                this, [this]() {
                    refreshDialogFromEngine();
                });
        connect(storyEngine_, &StoryEngine::scriptFinished,
                this, [this]() {
                    QString scriptPath =
                        QString(":/scripts/week%1.json")
                            .arg(GameManager::instance().player()->currentWeek());
                    GameManager::instance().player()->markScriptFinished(scriptPath);
                    GameManager::instance().requestScene(GameScene::Map);
                });
    }
    dialogIndex_ = stack_->addWidget(dialogPage);     // ★ 动态记录

            // ── 页面：小游戏大厅 ──────────────────────────────────
    auto* blackjackGame    = new BlackjackGame;
    auto* ticGame          = new TicTacToeGame;
    auto* mineGame         = new MinesweeperGame;
    auto* mazeGame         = new MazeGame;
    auto* memoryGame       = new MemoryGame;
    auto* miniGameMenuPage = new QWidget;
    {
        auto* layout = new QVBoxLayout(miniGameMenuPage);
        auto* title  = new QLabel(tr("小游戏中心"));
        title->setAlignment(Qt::AlignCenter);
        title->setStyleSheet("font-size:28px;font-weight:bold;color:#8B1A1A;");
        layout->addWidget(title);

        auto* blackjackBtn = new QPushButton(tr("21点"));
        auto* ticBtn       = new QPushButton(tr("井字棋"));
        auto* mineBtn      = new QPushButton(tr("扫雷"));
        auto* memoryBtn    = new QPushButton(tr("记忆翻牌"));
        auto* mazeBtn      = new QPushButton(tr("AI迷宫"));
        QString btnStyle =
            "QPushButton{font-size:20px;background:#F5E6CA;"
            "border-radius:10px;color:#4A0000;}"
            "QPushButton:hover{background:#e8d0a0;}";
        for (auto* b : {blackjackBtn, ticBtn, mineBtn, memoryBtn, mazeBtn}) {
            b->setMinimumHeight(60);
            b->setStyleSheet(btnStyle);
            layout->addWidget(b);
        }
        layout->addStretch();
        auto* backBtn = new QPushButton(tr("← 返回地图"));
        backBtn->setMinimumHeight(44);
        backBtn->setStyleSheet(btnStyle);
        connect(backBtn, &QPushButton::clicked, this, []() {
            GameManager::instance().requestScene(GameScene::Map);
        });
        layout->addWidget(backBtn);

        connect(blackjackBtn, &QPushButton::clicked, this,
                [this, blackjackGame]() {
                    blackjackGame->start();
                    stack_->setCurrentIndex(blackjackIndex_);
                });
        connect(ticBtn, &QPushButton::clicked, this,
                [this, ticGame]() {
                    ticGame->start();
                    stack_->setCurrentIndex(ticTacToeIndex_);
                });
        connect(mineBtn, &QPushButton::clicked, this,
                [this, mineGame]() {
                    mineGame->start();
                    stack_->setCurrentIndex(minesweeperIndex_);
                });
        connect(memoryBtn, &QPushButton::clicked, this,
                [this, memoryGame]() {
                    memoryGame->start();
                    stack_->setCurrentIndex(memoryIndex_);
                });
        connect(mazeBtn, &QPushButton::clicked, this,
                [this, mazeGame]() {
                    mazeGame->start();
                    stack_->setCurrentIndex(mazeIndex_);
                });
    }
    miniGameIndex_ = stack_->addWidget(miniGameMenuPage); // ★ 动态记录

            // ── 小游戏各页 ────────────────────────────────────────
    auto makeMiniPage = [&](QWidget* game, MiniGameType type, int& outIndex) {
        auto* page   = new QWidget;
        auto* layout = new QVBoxLayout(page);
        auto* back   = new QPushButton(tr("← 返回大厅"));
        back->setStyleSheet(
            "QPushButton{font-size:14px;background:#F5E6CA;"
            "border-radius:6px;color:#4A0000;}"
            "QPushButton:hover{background:#e8d0a0;}");
        if (auto* mg = qobject_cast<SA::MiniGame*>(game)) {
            connect(mg, &SA::MiniGame::finished,
                    this, [type](int score, bool won) {
                        GameManager::instance().onMiniGameFinished(
                            type, score, won);
                    });
        }
        connect(back, &QPushButton::clicked, this, [this]() {
            stack_->setCurrentIndex(miniGameIndex_);
        });
        layout->addWidget(game);
        layout->addWidget(back);
        outIndex = stack_->addWidget(page);            // ★ 动态记录
    };
    makeMiniPage(blackjackGame, MiniGameType::Blackjack,   blackjackIndex_);
    makeMiniPage(ticGame,       MiniGameType::TicTacToe,   ticTacToeIndex_);
    makeMiniPage(mineGame,      MiniGameType::Minesweeper, minesweeperIndex_);
    makeMiniPage(memoryGame,    MiniGameType::MemoryMatch,  memoryIndex_);
    makeMiniPage(mazeGame,      MiniGameType::Maze,         mazeIndex_);

            // ── 页面：结局 ────────────────────────────────────────
    auto* endingPage = new QWidget;
    {
        auto* layout = new QVBoxLayout(endingPage);
        auto* label  = new QLabel(
            tr("[ 结局演出 ]\n\n（由 EndingJudge 判定后展示对应结局）"));
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-size:28px;color:#8B1A1A;");
        auto* backBtn = new QPushButton(tr("← 返回主菜单"));
        backBtn->setStyleSheet(
            "QPushButton{font-size:16px;background:#F5E6CA;border-radius:8px;}"
            "QPushButton:hover{background:#e8d0a0;}");
        connect(backBtn, &QPushButton::clicked, this, []() {
            GameManager::instance().requestScene(GameScene::MainMenu);
        });
        layout->addStretch();
        layout->addWidget(label);
        layout->addStretch();
        layout->addWidget(backBtn);
        layout->setContentsMargins(60, 60, 60, 60);
    }
    endingIndex_ = stack_->addWidget(endingPage);     // ★ 动态记录

            // ── 页面：设置 ────────────────────────────────────────
    auto* settingsPage = new QWidget;
    settingsPage->setStyleSheet("background:#FFF8EE;");
    {
        auto* outer = new QVBoxLayout(settingsPage);
        outer->setContentsMargins(40, 30, 40, 30);
        outer->setSpacing(20);

        auto* titleLabel = new QLabel(tr("设  置"));
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet(
            "font-size:28px;font-weight:bold;color:#8B1A1A;");
        outer->addWidget(titleLabel);

        auto* attrGroup = new QGroupBox(tr("角色属性"));
        attrGroup->setStyleSheet(
            "QGroupBox{font-size:16px;font-weight:bold;color:#4A0000;"
            "border:2px solid #C8A070;border-radius:8px;margin-top:8px;}"
            "QGroupBox::title{subcontrol-origin:margin;padding:0 6px;}");
        auto* attrGrid  = new QGridLayout(attrGroup);
        attrGrid->setSpacing(10);

        auto* player2 = GameManager::instance().player();
        auto* dateLabel = new QLabel(
            tr("当前进度：第 %1 周·第 %2 天  |  本周剩余自由天数：%3 天")
                .arg(player2->currentWeek())
                .arg(player2->currentDay())
                .arg(player2->freeDaysLeft()));
        dateLabel->setStyleSheet("font-size:14px;color:#333;");
        attrGrid->addWidget(dateLabel, 0, 0, 1, 4);

        connect(player2, &Player::dateChanged, dateLabel,
                [dateLabel, player2](int w, int d) {
                    dateLabel->setText(
                        tr("当前进度：第 %1 周·第 %2 天  |  本周剩余自由天数：%3 天")
                            .arg(w).arg(d).arg(player2->freeDaysLeft()));
                });
        connect(player2, &Player::freeDaysChanged, dateLabel,
                [dateLabel, player2](int) {
                    dateLabel->setText(
                        tr("当前进度：第 %1 周·第 %2 天  |  本周剩余自由天数：%3 天")
                            .arg(player2->currentWeek())
                            .arg(player2->currentDay())
                            .arg(player2->freeDaysLeft()));
                });

        struct BarInfo { SubjectType type; QString name; QString color; };
        QList<BarInfo> barInfos = {
                                   {SubjectType::ProgDesign,    tr("程序设计好感"), "#E91E8C"},
                                   {SubjectType::Calculus,      tr("高数好感"),     "#2196F3"},
                                   {SubjectType::LinearAlgebra, tr("线代好感"),     "#9C27B0"},
                                   {SubjectType::AIIntro,       tr("AI引论好感"),   "#FF9800"},
                                   };
        int row = 1;
        for (auto& info : barInfos) {
            auto* lbl = new QLabel(info.name + ":", attrGroup);
            lbl->setStyleSheet("font-size:14px;");
            auto* bar = makeBar(info.color, attrGroup);
            bar->setValue(player2->affinity(info.type));
            connect(player2, &Player::affinityChanged,
                    bar, [bar, t = info.type](SubjectType s, int v) {
                        if (s == t) bar->setValue(v);
                    });
            attrGrid->addWidget(lbl, row, 0);
            attrGrid->addWidget(bar, row, 1, 1, 3);
            row++;
        }
        auto* stressLbl = new QLabel(tr("压力值:"), attrGroup);
        auto* darkLbl   = new QLabel(tr("黑化值:"), attrGroup);
        stressLbl->setStyleSheet("font-size:14px;");
        darkLbl->setStyleSheet("font-size:14px;");
        auto* stressBar = makeBar("#3B5BA5", attrGroup);
        auto* darkBar   = makeBar("#5A189A", attrGroup);
        stressBar->setValue(player2->stress());
        darkBar->setValue(player2->darkness());
        connect(player2, &Player::stressChanged,   stressBar, &QProgressBar::setValue);
        connect(player2, &Player::darknessChanged, darkBar,   &QProgressBar::setValue);
        attrGrid->addWidget(stressLbl, row,   0);
        attrGrid->addWidget(stressBar, row,   1, 1, 3);
        attrGrid->addWidget(darkLbl,   row+1, 0);
        attrGrid->addWidget(darkBar,   row+1, 1, 1, 3);
        outer->addWidget(attrGroup);

        auto* saveGroup = new QGroupBox(tr("存档管理"));
        saveGroup->setStyleSheet(
            "QGroupBox{font-size:16px;font-weight:bold;color:#4A0000;"
            "border:2px solid #C8A070;border-radius:8px;margin-top:8px;}"
            "QGroupBox::title{subcontrol-origin:margin;padding:0 6px;}");
        auto* saveLayout = new QHBoxLayout(saveGroup);
        auto* saveBtn    = new QPushButton(tr("💾  保存到存档 1"));
        auto* loadBtn    = new QPushButton(tr("📂  读取存档 1"));
        saveBtn->setStyleSheet(
            "QPushButton{font-size:16px;background:#8B1A1A;color:white;"
            "border-radius:8px;padding:10px 24px;}"
            "QPushButton:hover{background:#B22222;}");
        loadBtn->setStyleSheet(
            "QPushButton{font-size:16px;background:#3B5BA5;color:white;"
            "border-radius:8px;padding:10px 24px;}"
            "QPushButton:hover{background:#5070C0;}");
        saveBtn->setMinimumHeight(50);
        loadBtn->setMinimumHeight(50);
        connect(saveBtn, &QPushButton::clicked, this, [this]() {
            bool ok = SaveManager::save(GameManager::instance().player(), 1);
            QMessageBox::information(this,
                                     ok ? tr("保存成功") : tr("保存失败"),
                                     ok ? tr("已保存到存档 1") : tr("无法保存游戏"));
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
        outer->addWidget(saveGroup);
        outer->addStretch();

        auto* backBtn = new QPushButton(tr("← 返回"));
        backBtn->setMinimumHeight(44);
        backBtn->setStyleSheet(
            "QPushButton{font-size:16px;background:#F5E6CA;color:#4A0000;"
            "border-radius:8px;}"
            "QPushButton:hover{background:#e8d0a0;}");
        connect(backBtn, &QPushButton::clicked, this, [this]() {
            auto* p = GameManager::instance().player();
            if (p->currentWeek() == 1 && p->currentDay() == 1
                && !p->isScriptFinished(":/scripts/week1.json")) {
                stack_->setCurrentIndex(mainMenuIndex_);
            } else {
                stack_->setCurrentIndex(mapIndex_);
            }
        });
        outer->addWidget(backBtn);
    }
    settingsIndex_ = stack_->addWidget(settingsPage); // ★ 动态记录
}

// ════════════════════════════════════════════════════════════

void MainWindow::connectSignals() {
    connect(&GameManager::instance(), &GameManager::sceneChangeRequested,
            this, &MainWindow::onSceneChangeRequested);

    connect(GameManager::instance().player(), &Player::dateChanged,
            this, [this](int, int) { updateMapDateDisplay(); });
    connect(GameManager::instance().player(), &Player::freeDaysChanged,
            this, [this](int) { updateMapDateDisplay(); });

            // ★ 新增：进入新一周时自动触发对应剧本
    connect(GameManager::instance().player(), &Player::weekAdvanced,
            this, [this](int newWeek) {
                // 第 6 周以后没有剧本，触发结局判定
                if (newWeek > 5) {
                    GameManager::instance().requestScene(GameScene::Ending);
                    return;
                }
                QString scriptPath =
                    QString(":/scripts/week%1.json").arg(newWeek);
                // 新周剧本一定未完成，直接加载
                loadAndShowScript(scriptPath);
            });
}

void MainWindow::onSceneChangeRequested(GameScene scene) {
    switch (scene) {
        case GameScene::MainMenu: stack_->setCurrentIndex(mainMenuIndex_);  break;
        case GameScene::Map:
            updateMapDateDisplay();
            stack_->setCurrentIndex(mapIndex_);
            break;
        case GameScene::Dialog:   stack_->setCurrentIndex(dialogIndex_);    break;
        case GameScene::MiniGame: stack_->setCurrentIndex(miniGameIndex_);  break;
        case GameScene::Ending:   stack_->setCurrentIndex(endingIndex_);    break;
    }
}

void MainWindow::updateMapDateDisplay() {
    auto* player = GameManager::instance().player();
    if (mapDateLabel_) {
        mapDateLabel_->setText(
            tr("第 %1 周·第 %2 天")
                .arg(player->currentWeek())
                .arg(player->currentDay()));
    }
    if (mapFreeDayLabel_) {
        int left = player->freeDaysLeft();
        mapFreeDayLabel_->setText(
            left > 0
                ? tr("本周剩余自由活动：%1 天").arg(left)
                : tr("本周自由活动已用完"));
        mapFreeDayLabel_->setStyleSheet(
            left > 0
                ? "font-size:15px;font-weight:bold;color:#4A0000;"
                  "background:rgba(255,240,210,200);border-radius:6px;padding:4px 12px;"
                : "font-size:15px;font-weight:bold;color:#888;"
                  "background:rgba(220,220,220,200);border-radius:6px;padding:4px 12px;");
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
    QString  speaker  = storyEngine_->currentSpeaker();
    QString  text     = storyEngine_->currentText();
    QStringList choices  = storyEngine_->currentChoices();
    QString  sprite   = storyEngine_->currentSprite(); // ── 新增：从引擎拿到立绘标识 ──

            // 对话场景页
    dialogWindow_->setContent(speaker, text, choices);
    dialogWindow_->setSprite(sprite);                  // ── 新增：把立绘推入对话框渲染 ──
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_F12) { showDebugMenu(); return; }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::showDebugMenu() {
    auto* player = GameManager::instance().player();
    QDialog dialog(this);
    dialog.setWindowTitle("调试菜单");
    QFormLayout layout(&dialog);

    QSpinBox progBox, calBox, linearBox, aiBox, stressBox, darkBox;
    for (auto* b : {&progBox,&calBox,&linearBox,&aiBox,&stressBox,&darkBox})
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
    layout.addRow("AI引论好感",   &aiBox);
    layout.addRow("压力值",       &stressBox);
    //layout.addRow("线代黑化值",   &darkBox);

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

void MainWindow::handleLocationAction(Location loc) {
    auto* player = GameManager::instance().player();
    switch (loc) {
        case Location::Classroom: {
            QString scriptPath =
                QString(":/scripts/week%1.json").arg(player->currentWeek());
            if (player->isScriptFinished(scriptPath)) {
                QMessageBox::information(this, tr("教学楼"),
                                         tr("本周课程剧情已完成。\n可以去图书馆、未名湖或宿舍自由活动。"));
            } else {
                loadAndShowScript(scriptPath);
            }
            break;
        }
        case Location::Library:
            GameManager::instance().requestScene(GameScene::MiniGame);
            player->consumeFreeDay();
            break;
        case Location::WeimingLake:
            player->addStress(-5);
            player->consumeFreeDay();
            updateMapDateDisplay();
            break;
        case Location::Dormitory:
            player->addStress(-10);
            player->consumeFreeDay();
            updateMapDateDisplay();
            break;
    }
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    if (locationBgLabel_ && !locationBgLabel_->pixmap().isNull()) {
        locationBgLabel_->setScaledContents(true);
    }
}


} // namespace SA