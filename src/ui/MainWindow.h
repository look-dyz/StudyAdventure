/**
 * @file MainWindow.h
 * @brief 主窗口（使用 QStackedWidget 切换不同场景）
 *
 * 负责人：成员 B
 *
 * ===== 场景切换 =====
 * 监听 GameManager::sceneChangeRequested 信号，
 * 调用 QStackedWidget::setCurrentIndex 切换页面。
 */
#ifndef STUDYADVENTURE_MAINWINDOW_H
#define STUDYADVENTURE_MAINWINDOW_H

#include <QMainWindow>
#include "common/Constants.h"

class QStackedWidget;

namespace SA {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onSceneChangeRequested(GameScene scene);

private:
    QStackedWidget* stack_;

    // 各场景页面索引（在 setupUi 中按添加顺序对应）
    int mainMenuIndex_  = 0;
    int mapIndex_       = 1;
    int dialogIndex_    = 2;
    int miniGameIndex_  = 3;
    int endingIndex_    = 4;

    void setupUi();
    void connectSignals();
};

} // namespace SA

#endif // STUDYADVENTURE_MAINWINDOW_H
