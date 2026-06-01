/**
 * @file MainWindow.h
 * @brief 主窗口（使用 QStackedWidget 切换不同场景）
 *
 * 负责人：成员 B（场景切换框架）+ 成员 A（剧情接入）
 *
 * ===== 场景切换 =====
 * 监听 GameManager::sceneChangeRequested 信号，
 * 调用 QStackedWidget::setCurrentIndex 切换页面。
 *
 * ===== 剧情接入 =====
 * 持有 StoryEngine 和 DialogWindow 引用，
 * loadAndShowScript() 加载剧本 → 切到对话场景 → 信号槽自动刷新对话框
 */
#ifndef STUDYADVENTURE_MAINWINDOW_H
#define STUDYADVENTURE_MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include "common/Constants.h"

class QStackedWidget;

namespace SA {

class StoryEngine;
class DialogWindow;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

    /// 加载剧本并切到对话场景
    void loadAndShowScript(const QString& scriptPath);

private slots:
    void onSceneChangeRequested(GameScene scene);

private:
    QStackedWidget* stack_;
    StoryEngine* storyEngine_;
    DialogWindow* dialogWindow_;

    // 各场景页面索引
    int mainMenuIndex_  = 0;
    int mapIndex_       = 1;
    int dialogIndex_    = 2;
    int miniGameIndex_  = 3;
    int endingIndex_    = 4;

    int blackjackIndex_;
    int ticTacToeIndex_;
    int minesweeperIndex_;
    int memoryIndex_;
    int mazeIndex_;

    void setupUi();
    void connectSignals();

    /// 从 StoryEngine 取当前节点信息，刷新到 DialogWindow
    void refreshDialogFromEngine();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    void showDebugMenu();
};

} // namespace SA

#endif // STUDYADVENTURE_MAINWINDOW_H
