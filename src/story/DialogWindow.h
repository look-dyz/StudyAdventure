/**
 * @file DialogWindow.h
 * @brief 对话框窗口（显示头像、台词、选项按钮）
 *
 * 负责人：成员 A
 */
#ifndef STUDYADVENTURE_DIALOGWINDOW_H
#define STUDYADVENTURE_DIALOGWINDOW_H

#include <QWidget>

class QLabel;
class QPushButton;
class QVBoxLayout;

namespace SA {

class DialogWindow : public QWidget {
    Q_OBJECT
public:
    explicit DialogWindow(QWidget* parent = nullptr);

    /// 设置当前显示内容
    void setContent(const QString& speaker,
                    const QString& text,
                    const QStringList& choices);

signals:
    /// 玩家点击了某个选项
    void choiceMade(int index);

private:
    QLabel* speakerLabel_;
    QLabel* textLabel_;
    QVBoxLayout* choicesLayout_;
    QList<QPushButton*> choiceButtons_;

    void clearChoices();
};

} // namespace SA

#endif // STUDYADVENTURE_DIALOGWINDOW_H
