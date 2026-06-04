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
    void loadAndShowScript(const QString& scriptPath);

private slots:
    void onSceneChangeRequested(GameScene scene);

private:
    QStackedWidget* stack_;
    StoryEngine*    storyEngine_;
    DialogWindow*   dialogWindow_;

    int mainMenuIndex_   = 0;
    int mapIndex_        = 1;
    int dialogIndex_     = 2;
    int miniGameIndex_   = 3;
    int endingIndex_     = 4;
    int settingsIndex_   = -1;   // ← 新增
    int blackjackIndex_;
    int ticTacToeIndex_;
    int minesweeperIndex_;
    int memoryIndex_;
    int mazeIndex_;

    void setupUi();
    void connectSignals();
    void refreshDialogFromEngine();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    void showDebugMenu();
};
} // namespace SA
#endif