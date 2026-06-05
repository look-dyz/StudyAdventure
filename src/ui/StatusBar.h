/**
 * @file StatusBar.h
 * @brief 顶部状态栏，显示玩家所有数值
 *
 * 监听 Player 的各种信号，自动刷新进度条显示。
 * 负责人：成员 B
 */
#ifndef STUDYADVENTURE_STATUSBAR_H
#define STUDYADVENTURE_STATUSBAR_H

#include <QWidget>
#include <QMap>
#include "common/Constants.h"

class QLabel;
class QProgressBar;

namespace SA {

class Player;

class StatusBar : public QWidget {
    Q_OBJECT
public:
    explicit StatusBar(Player* player, QWidget* parent = nullptr);

private slots:
    void onAffinityChanged(SubjectType s, int value);
    void onStressChanged(int value);
    void onDarknessChanged(int value);
    void onDateChanged(int week, int day);

private:
    QMap<SubjectType, QProgressBar*> affinityBars_;
    QProgressBar* stressBar_ = nullptr;
    QProgressBar* darknessBar_ = nullptr;
    QLabel* dateLabel_ = nullptr;

    void buildUI();
    QProgressBar* createBar(const QString& color);
};

} // namespace SA

#endif // STUDYADVENTURE_STATUSBAR_H
