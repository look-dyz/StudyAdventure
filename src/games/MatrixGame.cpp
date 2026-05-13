#include "MatrixGame.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QDebug>
#include <cmath>

namespace SA {

// ========== Matrix 实现 ==========
Matrix::Matrix(int rows, int cols)
    : data_(rows, std::vector<double>(cols, 0)) {}

Matrix::Matrix(std::initializer_list<std::initializer_list<double>> init) {
    for (const auto& row : init) {
        data_.emplace_back(row);
    }
}

Matrix Matrix::operator+(const Matrix& other) const {
    Matrix result(rows(), cols());
    for (int r = 0; r < rows(); ++r)
        for (int c = 0; c < cols(); ++c)
            result.at(r, c) = at(r, c) + other.at(r, c);
    return result;
}

Matrix Matrix::operator-(const Matrix& other) const {
    Matrix result(rows(), cols());
    for (int r = 0; r < rows(); ++r)
        for (int c = 0; c < cols(); ++c)
            result.at(r, c) = at(r, c) - other.at(r, c);
    return result;
}

Matrix Matrix::operator*(const Matrix& other) const {
    Matrix result(rows(), other.cols());
    for (int r = 0; r < rows(); ++r) {
        for (int c = 0; c < other.cols(); ++c) {
            double sum = 0;
            for (int k = 0; k < cols(); ++k) {
                sum += at(r, k) * other.at(k, c);
            }
            result.at(r, c) = sum;
        }
    }
    return result;
}

Matrix Matrix::operator*(double scalar) const {
    Matrix result(rows(), cols());
    for (int r = 0; r < rows(); ++r)
        for (int c = 0; c < cols(); ++c)
            result.at(r, c) = at(r, c) * scalar;
    return result;
}

double Matrix::determinant() const {
    int n = rows();
    if (n == 1) return at(0, 0);
    if (n == 2) {
        return at(0, 0) * at(1, 1) - at(0, 1) * at(1, 0);
    }
    if (n == 3) {
        return at(0, 0) * (at(1, 1) * at(2, 2) - at(1, 2) * at(2, 1))
             - at(0, 1) * (at(1, 0) * at(2, 2) - at(1, 2) * at(2, 0))
             + at(0, 2) * (at(1, 0) * at(2, 1) - at(1, 1) * at(2, 0));
    }
    return 0;  // 不支持更高阶
}

int Matrix::rank() const {
    // TODO（成员 C）：通过初等行变换（高斯消元）计算秩
    return 0;
}

Matrix Matrix::transpose() const {
    Matrix result(cols(), rows());
    for (int r = 0; r < rows(); ++r)
        for (int c = 0; c < cols(); ++c)
            result.at(c, r) = at(r, c);
    return result;
}

QString Matrix::toString() const {
    QString s;
    for (int r = 0; r < rows(); ++r) {
        s += "[ ";
        for (int c = 0; c < cols(); ++c) {
            s += QString::number(at(r, c)) + " ";
        }
        s += "]\n";
    }
    return s;
}

// ========== MatrixGame 实现 ==========
MatrixGame::MatrixGame(QWidget* parent) : MiniGame(parent) {
    auto* layout = new QVBoxLayout(this);

    auto* title = new QLabel(displayName(), this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #8B1A1A;");

    progressLabel_ = new QLabel(this);
    timerLabel_ = new QLabel(this);
    timerLabel_->setStyleSheet("font-size: 16px; color: #C53030;");
    promptLabel_ = new QLabel(this);
    promptLabel_->setStyleSheet("font-family: 'Consolas', monospace; font-size: 14px;");
    promptLabel_->setWordWrap(true);

    answerEdit_ = new QLineEdit(this);
    submitBtn_ = new QPushButton(tr("提交"), this);
    connect(submitBtn_, &QPushButton::clicked, this, &MatrixGame::onSubmit);
    connect(answerEdit_, &QLineEdit::returnPressed, this, &MatrixGame::onSubmit);

    timer_ = new QTimer(this);
    timer_->setInterval(1000);
    connect(timer_, &QTimer::timeout, this, &MatrixGame::onTimerTick);

    layout->addWidget(title);
    layout->addWidget(progressLabel_);
    layout->addWidget(timerLabel_);
    layout->addWidget(promptLabel_);
    layout->addWidget(answerEdit_);
    layout->addWidget(submitBtn_);
    layout->addStretch();
}

void MatrixGame::start() {
    reset();
}

void MatrixGame::reset() {
    questions_.clear();
    currentIdx_ = 0;
    correctCount_ = 0;
    generateQuestions();
    showQuestion();
    timer_->start();
}

void MatrixGame::generateQuestions() {
    // TODO（成员 C）：生成 TOTAL_QUESTIONS 道题
    // 各题型按比例：3 道加法、3 道乘法、2 道行列式、2 道秩
    // 注意控制系数范围（-5 ~ 5），保证答案是整数
    for (int i = 0; i < TOTAL_QUESTIONS; ++i) {
        questions_.push_back(generateOne(MatrixQuestion::Add));
    }
}

MatrixQuestion MatrixGame::generateOne(MatrixQuestion::Type t) {
    MatrixQuestion q;
    q.type = t;
    // TODO（成员 C）：根据类型生成具体题目
    q.prompt = tr("（待生成的题目）");
    q.answer = 0;
    return q;
}

void MatrixGame::showQuestion() {
    if (currentIdx_ >= TOTAL_QUESTIONS) {
        endGame();
        return;
    }
    const auto& q = questions_[currentIdx_];
    progressLabel_->setText(tr("题目 %1 / %2").arg(currentIdx_ + 1).arg(TOTAL_QUESTIONS));
    promptLabel_->setText(q.prompt);
    answerEdit_->clear();
    answerEdit_->setFocus();
    remainingSeconds_ = TIME_PER_QUESTION;
    timerLabel_->setText(tr("剩余 %1 秒").arg(remainingSeconds_));
}

void MatrixGame::onSubmit() {
    bool ok = false;
    double userAnswer = answerEdit_->text().toDouble(&ok);
    if (ok && std::abs(userAnswer - questions_[currentIdx_].answer) < 1e-6) {
        correctCount_++;
    }
    nextQuestion();
}

void MatrixGame::onTimerTick() {
    remainingSeconds_--;
    timerLabel_->setText(tr("剩余 %1 秒").arg(remainingSeconds_));
    if (remainingSeconds_ <= 0) {
        nextQuestion();
    }
}

void MatrixGame::nextQuestion() {
    currentIdx_++;
    showQuestion();
}

void MatrixGame::endGame() {
    timer_->stop();
    int score = correctCount_ * 10;  // 每题 10 分
    bool won = correctCount_ >= 7;
    promptLabel_->setText(tr("结束！答对 %1 / %2").arg(correctCount_).arg(TOTAL_QUESTIONS));
    answerEdit_->setEnabled(false);
    submitBtn_->setEnabled(false);

    // 失败时增加线代黑化值（在 GameManager 中处理）
    emit finished(score, won);
}

} // namespace SA
