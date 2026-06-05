#ifndef STUDYADVENTURE_MAINMENU_H
#define STUDYADVENTURE_MAINMENU_H
#include <QWidget>
#include <QResizeEvent>

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
protected:
    void resizeEvent(QResizeEvent* event) override;
};
} // namespace SA
#endif