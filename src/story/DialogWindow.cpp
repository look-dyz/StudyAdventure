#include "DialogWindow.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace SA {

DialogWindow::DialogWindow(QWidget* parent)
    : QWidget(parent),
      speakerLabel_(new QLabel(this)),
      textLabel_(new QLabel(this)),
      choicesLayout_(new QVBoxLayout)
{
    // 基础布局：上方说话人姓名 + 中间台词 + 下方选项
    auto* mainLayout = new QVBoxLayout(this);

    speakerLabel_->setStyleSheet("font-weight: bold; font-size: 16px; color: #8B1A1A;");
    textLabel_->setWordWrap(true);
    textLabel_->setStyleSheet("font-size: 14px; padding: 8px;");
    textLabel_->setMinimumHeight(80);

    mainLayout->addWidget(speakerLabel_);
    mainLayout->addWidget(textLabel_);
    mainLayout->addLayout(choicesLayout_);
    mainLayout->addStretch();

    setLayout(mainLayout);
}

void DialogWindow::setContent(const QString& speaker,
                              const QString& text,
                              const QStringList& choices) {
    speakerLabel_->setText(speaker);
    textLabel_->setText(text);

    clearChoices();
    for (int i = 0; i < choices.size(); ++i) {
        auto* btn = new QPushButton(choices[i], this);
        const int idx = i;  // 捕获副本
        connect(btn, &QPushButton::clicked, this, [this, idx]() {
            emit choiceMade(idx);
        });
        choicesLayout_->addWidget(btn);
        choiceButtons_.append(btn);
    }
}

void DialogWindow::clearChoices() {
    for (auto* btn : choiceButtons_) {
        choicesLayout_->removeWidget(btn);
        btn->deleteLater();
    }
    choiceButtons_.clear();
}

} // namespace SA
