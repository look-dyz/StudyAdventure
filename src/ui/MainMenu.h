/**
 * @file MainMenu.h
 * @brief 主菜单界面（独立组件，便于美化）
 *
 * 负责人：成员 B
 */
#ifndef STUDYADVENTURE_MAINMENU_H
#define STUDYADVENTURE_MAINMENU_H

#include <QWidget>

namespace SA {

class MainMenu : public QWidget {
    Q_OBJECT
public:
    explicit MainMenu(QWidget* parent = nullptr);

signals:
    void startNewGameClicked();
    void loadGameClicked();
    void settingsClicked();
    void exitClicked();
};

} // namespace SA

#endif // STUDYADVENTURE_MAINMENU_H
