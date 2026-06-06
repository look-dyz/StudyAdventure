/**
 * @file MatrixGame.h
 * @brief 矩阵运算小游戏（对应线代学科）
 *
 * 玩法：限时答题，10 题，覆盖矩阵加法、乘法、行列式、求秩等。
 * OOP 重点：Matrix 类的运算符重载，是大一 C++ 课的经典练习。
 *
 * 负责人：成员 C   预计完成：5/23
 */
#ifndef STUDYADVENTURE_MATRIXGAME_H
#define STUDYADVENTURE_MATRIXGAME_H

#include "common/MiniGame.h"
#include <QString>
#include <vector>

class QLabel;
class QLineEdit;
class QPushButton;
class QTimer;

namespace SA {

// ========== Matrix 类（演示运算符重载）==========
class Matrix {
public:
    Matrix() = default;
    Matrix(int rows, int cols);
    Matrix(std::initializer_list<std::initializer_list<double>> init);

    int rows() const { return static_cast<int>(data_.size()); }
    int cols() const { return data_.empty() ? 0 : static_cast<int>(data_[0].size()); }

    double& at(int r, int c) { return data_[r][c]; }
    double at(int r, int c) const { return data_[r][c]; }

    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;
    Matrix operator*(const Matrix& other) const;
    Matrix operator*(double scalar) const;

    double determinant() const;   ///< 仅支持 1x1 / 2x2 / 3x3
    int rank() const;              ///< 通过初等行变换计算
    Matrix transpose() const;

    QString toString() const;

private:
    std::vector<std::vector<double>> data_;
};

// ========== 题目类型 ==========
struct MatrixQuestion {
    enum Type { Add, Multiply, Determinant, Rank, IsInvertible };
    Type type;
    Matrix m1;
    Matrix m2;
    int targetRow = 0;    ///< 对于 Add/Multiply，问的是哪个位置的元素
    int targetCol = 0;
    double answer = 0;
    QString prompt;
};

// ========== 主游戏类 ==========
class MatrixGame : public MiniGame {
    Q_OBJECT
public:
    explicit MatrixGame(QWidget* parent = nullptr);
    ~MatrixGame() override = default;

    void start() override;
    void reset() override;
    MiniGameType type() const override { return MiniGameType::Matrix; }
    QString displayName() const override { return tr("矩阵运算"); }

private slots:
    void onSubmit();
    void onTimerTick();

private:
    static constexpr int TOTAL_QUESTIONS = 10;
    static constexpr int TIME_PER_QUESTION = 30;  // 秒

    std::vector<MatrixQuestion> questions_;
    int currentIdx_ = 0;
    int correctCount_ = 0;
    int remainingSeconds_ = TIME_PER_QUESTION;

    QLabel* promptLabel_ = nullptr;
    QLabel* timerLabel_ = nullptr;
    QLabel* progressLabel_ = nullptr;
    QLineEdit* answerEdit_ = nullptr;
    QPushButton* submitBtn_ = nullptr;
    QTimer* timer_ = nullptr;

    void generateQuestions();
    MatrixQuestion generateOne(MatrixQuestion::Type t);
    void showQuestion();
    void nextQuestion();
    void endGame();
};

} // namespace SA

#endif // STUDYADVENTURE_MATRIXGAME_H
