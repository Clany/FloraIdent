#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#if USE_QT_VER >= 5
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif
#include "ui_settings_dialog.h"
#include "common.h"

_CLANY_BEGIN
struct FloraIdentSettings;
_CLANY_END

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    SettingsDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

    void init(const clany::FloraIdentSettings& settings);
    clany::FloraIdentSettings getSettings();

public slots:
    void accept() override;

private:
    bool parseOrientaion(std::vector<int>& orients);
    void parseOrientaion(const std::vector<int>& orients, std::string& orient_str);

    Ui::SettingsDialogClass ui;
};

#endif // SETTINGS_DIALOG_H