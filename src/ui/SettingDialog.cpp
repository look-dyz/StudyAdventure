#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

namespace SA {

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("游戏设置 - 异世界终端"));
    resize(400, 300);
    setupUi();
    loadSettings();
}

void SettingsDialog::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);

            // 音量调节
    auto* musicLayout = new QHBoxLayout();
    musicLayout->addWidget(new QLabel(tr("背景音乐 (BGM):"), this));
    musicSlider_ = new QSlider(Qt::Horizontal, this);
    musicSlider_->setRange(0, 100);
    musicLayout->addWidget(musicSlider_);
    mainLayout->addLayout(musicLayout);

    auto* sfxLayout = new QHBoxLayout();
    sfxLayout->addWidget(new QLabel(tr("音效体积 (SFX):"), this));
    sfxSlider_ = new QSlider(Qt::Horizontal, this);
    sfxSlider_->setRange(0, 100);
    sfxLayout->addWidget(sfxSlider_);
    mainLayout->addLayout(sfxLayout);

            // 语言选择
    auto* langLayout = new QHBoxLayout();
    langLayout->addWidget(new QLabel(tr("显示语言:"), this));
    langCombo_ = new QComboBox(this);
    langCombo_->addItems({tr("简体中文"), tr("English (En_US)")});
    langLayout->addWidget(langCombo_);
    mainLayout->addLayout(langLayout);

            // 全屏显示
    fullscreenCheck_ = new QCheckBox(tr("全屏模式 (可能会导致期末考卷黑屏)"), this);
    mainLayout->addWidget(fullscreenCheck_);

    mainLayout->addStretch();

            // 底部按钮
    auto* btnLayout = new QHBoxLayout();
    auto* saveBtn = new QPushButton(tr("保存配置"), this);
    auto* cancelBtn = new QPushButton(tr("取消"), this);
    btnLayout->addStretch();
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(cancelBtn);
    mainLayout->addLayout(btnLayout);

    connect(saveBtn, &QPushButton::clicked, this, &SettingsDialog::saveSettings);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

            // 美化样式表
    setStyleSheet(R"(
        QDialog { background-color: #F8F9FA; }
        QLabel { font-size: 14px; color: #333; font-weight: bold; }
        QPushButton { padding: 6px 15px; border-radius: 4px; border: 1px solid #ccc; background: white; }
        QPushButton:hover { background: #E9ECEF; }
    )");
}

void SettingsDialog::loadSettings() {
    // 默认初始值，后续可接入 QSettings
    musicSlider_->setValue(80);
    sfxSlider_->setValue(90);
}

void SettingsDialog::saveSettings() {
    // 触发保存逻辑（可通知成员 C 的 AudioManager 调节全局音量）
    accept();
}

} // namespace SA
