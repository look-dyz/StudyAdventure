#ifndef STUDYADVENTURE_MAINWINDOW_H
#define STUDYADVENTURE_MAINWINDOW_H
#include <QMainWindow>
#include <QKeyEvent>
#include <QLabel>
#include "common/Constants.h"

class QStackedWidget;
namespace SA {
class StoryEngine;
class DialogWindow;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    void loadAndShowScript(const QString& scriptPath);

private slots:
    void onSceneChangeRequested(GameScene scene);

private:
    QStackedWidget* stack_;
    StoryEngine*    storyEngine_;
    DialogWindow*   dialogWindow_;

            // 地图页天数显示标签
    QLabel* mapDateLabel_    = nullptr;
    QLabel* mapFreeDayLabel_ = nullptr;

    int mainMenuIndex_   = 0;
    int mapIndex_        = 1;
    int dialogIndex_     = 2;
    int miniGameIndex_   = 3;
    int endingIndex_     = -1;
    int settingsIndex_   = -1;
    int blackjackIndex_;
    int ticTacToeIndex_;
    int minesweeperIndex_;
    int memoryIndex_;
    int mazeIndex_;

    void setupUi();
    void connectSignals();
    void refreshDialogFromEngine();
    void updateMapDateDisplay();   // 新增：刷新地图天数显示

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    void showDebugMenu();
};
} // namespace SA
#endif