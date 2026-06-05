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
    // ── 背景图 ──
    bgLabel_->setObjectName("bgLabel");
    bgLabel_->setScaledContents(true);
    bgLabel_->setPixmap(QPixmap(":/images/backgrounds/plotbackground.jpg"));
    bgLabel_->lower();

    // ── 配置立绘标签 ──
    spriteLabel_->setObjectName("spriteLabel");
    spriteLabel_->setStyleSheet("background: transparent;");
    spriteLabel_->setAlignment(Qt::AlignCenter);
    spriteLabel_->hide();

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

// ── 💡 核心改动：固定高度算法 ──
void DialogWindow::adjustSpriteLayout() {
    if (currentSpritePix_.isNull()) return;

    // 1. 强行规定：所有立绘的【高度完全相同】（这里统一占全屏高度的 75%）
    // 不管是 3:4 还是 1:1，它们在这个高度下像素比例是 1:1 绝对平等的
    int finalH = height() * 0.75;

    // 2. 根据各自图片的原始比例，让宽度去自适应
    double ratio = static_cast<double>(currentSpritePix_.width()) / currentSpritePix_.height();
    int finalW = static_cast<int>(finalH * ratio);

    // 3. 计算水平居中 X 坐标
    int finalX = (width() - finalW) / 2;

    // 4. 计算 Y 坐标（让它们在屏幕中垂直居中，并往上提 30 像素避开底部的对话框）
    // 因为高度 finalH 统一了，所以它们的顶部和底部在视觉上会绝对对齐
    int finalY = (height() - finalH) / 2 - 30;

    // 📌 【高度控制台】
    // 如果觉得所有立绘都偏高或偏低，调节下面这行即可：
    // finalY -= 20; // 整体再往上提 20 像素

    // 5. 应用布局并无损刷新
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

    // 按钮样式
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