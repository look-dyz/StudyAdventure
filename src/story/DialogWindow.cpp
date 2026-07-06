#include "DialogWindow.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPixmap>
#include <QFrame>
#include <QDebug>

namespace SA {

DialogWindow::DialogWindow(QWidget* parent)
    : QWidget(parent),
      bgLabel_(new QLabel(this)),
      spriteLabel_(new QLabel(this)),
      speakerLabel_(new QLabel(this)),
      textLabel_(new QLabel(this)),
      choicesLayout_(new QVBoxLayout)
{
    // 背景图
    bgLabel_->setObjectName("bgLabel");
    bgLabel_->setScaledContents(true);
    bgLabel_->setPixmap(QPixmap(":/images/backgrounds/plotbackground.jpg"));
    bgLabel_->lower();

    // 配置立绘标签
    spriteLabel_->setObjectName("spriteLabel");
    spriteLabel_->setStyleSheet("background: transparent;");
    spriteLabel_->setAlignment(Qt::AlignCenter);
    spriteLabel_->hide();

    // 对话框底板
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

    // 主布局
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch();
    mainLayout->addWidget(panel);
    mainLayout->setContentsMargins(40, 0, 40, 30);

    setLayout(mainLayout);
}

void DialogWindow::adjustSpriteLayout() {
    if (currentSpritePix_.isNull()) return;

    // 统一占全屏高度的 75%
    int finalH = height() * 0.75;

    // 根据各自图片的原始比例计算宽度
    double ratio = static_cast<double>(currentSpritePix_.width()) / currentSpritePix_.height();
    int finalW = static_cast<int>(finalH * ratio);

    // 计算坐标
    int finalX = (width() - finalW) / 2;
    int finalY = (height() - finalH) / 2 - 30;

    spriteLabel_->setGeometry(finalX, finalY, finalW, finalH);
    spriteLabel_->setPixmap(currentSpritePix_.scaled(spriteLabel_->size(),
                                                     Qt::KeepAspectRatio,
                                                     Qt::SmoothTransformation));
}

void DialogWindow::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    bgLabel_->setGeometry(0, 0, width(), height());
    adjustSpriteLayout();
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

// ── 更换立绘控制函数 ──
void DialogWindow::setSprite(const QString& spriteKey) {
    if (spriteKey.isEmpty()) {
        currentSpritePix_ = QPixmap();
        spriteLabel_->hide();
        return;
    }

    QString path = spriteKey;
    if (!path.contains(":/")) {
        path = QString(":/images/characters/%1.png").arg(spriteKey);
    }

    currentSpritePix_ = QPixmap(path);

    if (currentSpritePix_.isNull()) {
        qWarning() << "[DialogWindow] Failed to load sprite image:" << path;
        spriteLabel_->hide();
    } else {
        adjustSpriteLayout();
        spriteLabel_->show();

        // 层级保持：bg -> sprite -> dialog
        spriteLabel_->lower();
        bgLabel_->lower();
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