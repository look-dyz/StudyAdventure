#include "DialogWindow.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPixmap>
#include <QFrame>

namespace SA {

DialogWindow::DialogWindow(QWidget* parent)
    : QWidget(parent),
      bgLabel_(new QLabel(this)),
      speakerLabel_(new QLabel(this)),
      textLabel_(new QLabel(this)),
      choicesLayout_(new QVBoxLayout)
{
    // ── 背景图 ──
    bgLabel_->setObjectName("bgLabel");
    bgLabel_->setScaledContents(true);
    bgLabel_->setPixmap(QPixmap(":/images/backgrounds/plotbackground.jpg"));
    bgLabel_->lower();

    // ── 对话框底板（半透明白框）──
    auto* panel = new QFrame(this);
    panel->setObjectName("dialogPanel");
    panel->setStyleSheet(
        "QFrame#dialogPanel {"
        "  background: rgba(255,255,255,200);"
        "  border-radius: 12px;"
        "}"
    );

    auto* panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(20, 16, 20, 16);
    panelLayout->setSpacing(10);

    speakerLabel_->setStyleSheet(
        "font-weight: bold; font-size: 18px; color: #8B1A1A;"
        "background: transparent;"
    );
    textLabel_->setWordWrap(true);
    textLabel_->setStyleSheet(
        "font-size: 15px; padding: 8px; background: transparent;"
    );
    textLabel_->setMinimumHeight(80);

    panelLayout->addWidget(speakerLabel_);
    panelLayout->addWidget(textLabel_);
    panelLayout->addLayout(choicesLayout_);

    // ── 主布局：面板贴底部 ──
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch();
    mainLayout->addWidget(panel);
    mainLayout->setContentsMargins(40, 0, 40, 30);

    setLayout(mainLayout);
}

void DialogWindow::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    bgLabel_->setGeometry(0, 0, width(), height());
}

void DialogWindow::setContent(const QString& speaker,
                              const QString& text,
                              const QStringList& choices) {
    speakerLabel_->setText(speaker);
    textLabel_->setText(text);
    clearChoices();

    QString btnStyle =
        "QPushButton {"
        "  font-size: 14px;"
        "  background: rgba(139,26,26,180);"
        "  color: white;"
        "  border-radius: 6px;"
        "  padding: 6px 12px;"
        "}"
        "QPushButton:hover { background: rgba(180,50,50,220); }";

    for (int i = 0; i < choices.size(); ++i) {
        auto* btn = new QPushButton(choices[i], this);
        btn->setStyleSheet(btnStyle);
        const int idx = i;
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