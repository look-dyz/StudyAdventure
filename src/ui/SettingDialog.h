#ifndef STUDYADVENTURE_SETTINGSDIALOG_H
#define STUDYADVENTURE_SETTINGSDIALOG_H

#include <QDialog>
#include <QSlider>
#include <QComboBox>
#include <QCheckBox>

namespace SA {

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget* parent = nullptr);

private:
    void setupUi();
    void loadSettings();
    void saveSettings();

    QSlider* musicSlider_;
    QSlider* sfxSlider_;
    QComboBox* langCombo_;
    QCheckBox* fullscreenCheck_;
};

} // namespace SA

#endif // STUDYADVENTURE_SETTINGSDIALOG_H
