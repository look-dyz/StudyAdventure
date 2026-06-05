/**
 * @file CalculusGame.h
 * @brief 高数答题小游戏（对应高数学科）
 *
 * 玩法：从 JSON 题库抽 10 道题，每题 20 秒，含连击系统。
 * 数据驱动：题库外置 assets/data/calculus_questions.json，便于扩充。
 *
 * 负责人：成员 C   预计完成：5/26
 */
#ifndef STUDYADVENTURE_CALCULUSGAME_H
#define STUDYADVENTURE_CALCULUSGAME_H

#include "common/MiniGame.h"
#include <QString>
#include <QStringList>
#include <vector>

class QLabel;
class QPushButton;
class QTimer;
class QVBoxLayout;

namespace SA {

struct CalcQuestion {
    QString id;
    QString topic;
    int difficulty = 1;
    QString prompt;
    QStringList options;
    int answer = 0;          ///< 正确答案的选项索引
    QString explanation;
};

class CalculusGame : public MiniGame {
    Q_OBJECT
public:
    explicit CalculusGame(QWidget* parent = nullptr);
    ~CalculusGame() override = default;

    void start() override;
    void reset() override;
    MiniGameType type() const override { return MiniGameType::Calculus; }
    QString displayName() const override { return tr("高数答题"); }

private slots:
    void onOptionClicked(int idx);
    void onTimerTick();

private:
    static constexpr int QUESTIONS_PER_GAME = 10;
    static constexpr int TIME_PER_QUESTION = 20;

    std::vector<CalcQuestion> questionBank_;       ///< 全部题库
    std::vector<CalcQuestion> currentQuestions_;   ///< 本局抽取的题
    int currentIdx_ = 0;
    int correctCount_ = 0;
    int comboCount_ = 0;             ///< 当前连击数
    int totalScore_ = 0;
    int remainingSeconds_ = TIME_PER_QUESTION;

    QLabel* progressLabel_ = nullptr;
    QLabel* timerLabel_ = nullptr;
    QLabel* comboLabel_ = nullptr;
    QLabel* promptLabel_ = nullptr;
    QLabel* explanationLabel_ = nullptr;
    QVBoxLayout* optionsLayout_ = nullptr;
    QList<QPushButton*> optionButtons_;
    QTimer* timer_ = nullptr;

    bool loadBank(const QString& jsonPath);
    void selectQuestions();
    void showQuestion();
    void clearOptions();
    void nextQuestion();
    void endGame();
};

} // namespace SA

#endif // STUDYADVENTURE_CALCULUSGAME_H
