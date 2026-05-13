/**
 * @file main.cpp
 * @brief 程序入口
 */
#include <QApplication>
#include "ui/MainWindow.h"
#include "core/GameManager.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // 应用元信息（用于 SaveManager 的 QStandardPaths）
    app.setOrganizationName("PKU-EECS");
    app.setApplicationName("StudyAdventure");

    // 初始化游戏主控（单例，第一次访问时构造）
    SA::GameManager::instance();

    // 显示主窗口
    SA::MainWindow window;
    window.show();

    return app.exec();
}
