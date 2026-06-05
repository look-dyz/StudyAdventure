#ifndef STUDYADVENTURE_DIALOGWINDOW_H
#define STUDYADVENTURE_DIALOGWINDOW_H
#include <QWidget>
#include <QResizeEvent>
class QLabel;
class QPushButton;
class QVBoxLayout;

namespace SA {
class DialogWindow : public QWidget {
    Q_OBJECT
public:
    explicit DialogWindow(QWidget* parent = nullptr);
    void setContent(const QString& speaker,
                    const QString& text,
                    const QStringList& choices);
signals:
    void choiceMade(int index);
protected:
    void resizeEvent(QResizeEvent* event) override;
private:
    QLabel* bgLabel_;          // 背景图
    QLabel* speakerLabel_;
    QLabel* textLabel_;
    QVBoxLayout* choicesLayout_;
    QList<QPushButton*> choiceButtons_;
    void clearChoices();
};
} // namespace SA
#endif