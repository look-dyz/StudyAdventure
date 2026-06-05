#include "CalculusGame.h"
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <random>
#include <algorithm>

namespace SA {

CalculusGame::CalculusGame(QWidget* parent) : MiniGame(parent) {
    auto* layout = new QVBoxLayout(this);

    auto* title = new QLabel(displayName(), this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #8B1A1A;");

    progressLabel_ = new QLabel(this);
    timerLabel_ = new QLabel(this);
    timerLabel_->setStyleSheet("font-size: 14px; color: #C53030;");
    comboLabel_ = new QLabel(this);
    comboLabel_->setStyleSheet("font-size: 14px; color: #2D6A4F;");

    promptLabel_ = new QLabel(this);
    promptLabel_->setStyleSheet("font-size: 15px; padding: 12px;");
    promptLabel_->setWordWrap(true);
    promptLabel_->setMinimumHeight(80);

    optionsLayout_ = new QVBoxLayout;

    explanationLabel_ = new QLabel(this);
    explanationLabel_->setStyleSheet("font-size: 12px; color: #666; font-style: italic;");
    explanationLabel_->setWordWrap(true);

    timer_ = new QTimer(this);
    timer_->setInterval(1000);
    connect(timer_, &QTimer::timeout, this, &CalculusGame::onTimerTick);

    layout->addWidget(title);
    layout->addWidget(progressLabel_);
    layout->addWidget(timerLabel_);
    layout->addWidget(comboLabel_);
    layout->addWidget(promptLabel_);
    layout->addLayout(optionsLayout_);
    layout->addWidget(explanationLabel_);
    layout->addStretch();
}

void CalculusGame::start() {
    // 加载题库（实际部署时应通过资源路径 :/data/calculus_questions.json）
    if (questionBank_.empty()) {
        if (!loadBank(":/data/calculus_questions.json")) {
            // 回退到文件系统路径（开发期间未打包资源）
            loadBank("assets/data/calculus_questions.json");
        }
    }
    reset();
}

bool CalculusGame::loadBank(const QString& jsonPath) {
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "[CalculusGame] Cannot open question bank:" << jsonPath;
        return false;
    }
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &err);
    if (err.error != QJsonParseError::NoError) {
        qWarning() << "[CalculusGame] Parse error:" << err.errorString();
        return false;
    }
    QJsonArray arr = doc.object().value("questions").toArray();
    for (const auto& v : arr) {
        QJsonObject o = v.toObject();
        CalcQuestion q;
        q.id = o.value("id").toString();
        q.topic = o.value("topic").toString();
        q.difficulty = o.value("difficulty").toInt(1);
        q.prompt = o.value("prompt").toString();
        for (const auto& opt : o.value("options").toArray()) {
            q.options.append(opt.toString());
        }
        q.answer = o.value("answer").toInt();
        q.explanation = o.value("explanation").toString();
        questionBank_.push_back(q);
    }
    qDebug() << "[CalculusGame] Loaded" << questionBank_.size() << "questions";
    return true;
}

void CalculusGame::reset() {
    currentQuestions_.clear();
    currentIdx_ = 0;
    correctCount_ = 0;
    comboCount_ = 0;
    totalScore_ = 0;
    selectQuestions();
    showQuestion();
    timer_->start();
}

void CalculusGame::selectQuestions() {
    if (questionBank_.empty()) {
        qWarning() << "[CalculusGame] Question bank empty!";
        return;
    }
    // 简单实现：随机抽题
    auto bank = questionBank_;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::shuffle(bank.begin(), bank.end(), gen);
    int n = std::min(QUESTIONS_PER_GAME, static_cast<int>(bank.size()));
    for (int i = 0; i < n; ++i) {
        currentQuestions_.push_back(bank[i]);
    }
}

void CalculusGame::showQuestion() {
    if (currentIdx_ >= static_cast<int>(currentQuestions_.size())) {
        endGame();
        return;
    }
    const auto& q = currentQuestions_[currentIdx_];
    progressLabel_->setText(tr("题目 %1 / %2").arg(currentIdx_ + 1).arg(currentQuestions_.size()));
    comboLabel_->setText(tr("连击 ×%1").arg(comboCount_));
    promptLabel_->setText(q.prompt);
    explanationLabel_->clear();

    clearOptions();
    for (int i = 0; i < q.options.size(); ++i) {
        auto* btn = new QPushButton(q.options[i], this);
        btn->setStyleSheet("text-align: left; padding: 8px;");
        const int idx = i;
        connect(btn, &QPushButton::clicked, this, [this, idx]() { onOptionClicked(idx); });
        optionsLayout_->addWidget(btn);
        optionButtons_.append(btn);
    }

    remainingSeconds_ = TIME_PER_QUESTION;
    timerLabel_->setText(tr("剩余 %1 秒").arg(remainingSeconds_));
}

void CalculusGame::clearOptions() {
    for (auto* btn : optionButtons_) {
        optionsLayout_->removeWidget(btn);
        btn->deleteLater();
    }
    optionButtons_.clear();
}

void CalculusGame::onOptionClicked(int idx) {
    const auto& q = currentQuestions_[currentIdx_];
    bool correct = (idx == q.answer);
    if (correct) {
        correctCount_++;
        comboCount_++;
        // 连击加成：基础 10 分 × (1 + 连击数 × 0.2)，上限 3 倍
        double mult = std::min(3.0, 1.0 + comboCount_ * 0.2);
        totalScore_ += static_cast<int>(10 * mult);
    } else {
        comboCount_ = 0;
        explanationLabel_->setText(tr("解析：%1").arg(q.explanation));
    }
    nextQuestion();
}

void CalculusGame::onTimerTick() {
    remainingSeconds_--;
    timerLabel_->setText(tr("剩余 %1 秒").arg(remainingSeconds_));
    if (remainingSeconds_ <= 0) {
        comboCount_ = 0;
        nextQuestion();
    }
}

void CalculusGame::nextQuestion() {
    currentIdx_++;
    showQuestion();
}

void CalculusGame::endGame() {
    timer_->stop();
    bool won = correctCount_ >= 7;
    promptLabel_->setText(tr("结束！答对 %1 / %2，总分 %3")
                          .arg(correctCount_).arg(currentQuestions_.size()).arg(totalScore_));
    clearOptions();
    emit finished(totalScore_, won);
}

} // namespace SA
