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

    QLabel* mapDateLabel_      = nullptr;
    QLabel* mapFreeDayLabel_   = nullptr;
    QLabel* locationBgLabel_   = nullptr;
    QLabel* locationDescLabel_ = nullptr;

            // 全部动态记录，不硬编码
    int mainMenuIndex_      = -1;
    int mapIndex_           = -1;
    int locationPageIndex_  = -1;
    int dialogIndex_        = -1;
    int miniGameIndex_      = -1;
    int endingIndex_        = -1;
    int settingsIndex_      = -1;
    int blackjackIndex_     = -1;
    int ticTacToeIndex_     = -1;
    int minesweeperIndex_   = -1;
    int memoryIndex_        = -1;
    int mazeIndex_          = -1;

    Location pendingLocation_ = Location::Classroom;

    void setupUi();
    void connectSignals();
    void refreshDialogFromEngine();
    void updateMapDateDisplay();
    void handleLocationAction(Location loc);
    void afterLoad();   // 读档后统一跳转逻辑

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void showDebugMenu();
};

} // namespace SA
#endif // STUDYADVENTURE_MAINWINDOW_H