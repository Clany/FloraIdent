#ifndef SETTINGS_DIALOG_H
#define SETTINGS_DIALOG_H

#include "flora_ident_config.h"
#if USE_QT_VER >= 5
#include <QtWidgets/QWidget>
#else
#include <QtGui/QWidget>
#endif
#include "ui_settings_dialog.h"
#include "flora_ident_app.h"
#include "common.h"

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    SettingsDialog(QWidget* parent = 0, Qt::WindowFlags f = 0);

    SettingsDialog(QWidget* parent, Qt::WindowFlags f,
                   const clany::FloraIdentSettings& settings);

    void init(const clany::FloraIdentSettings& settings);
    clany::FloraIdentSettings getSettings();

public slots:
    void accept() override;

private:
    bool parseOrientaion(std::vector<int>& orients);
    void parseOrientaion(const std::vector<int>& orients, std::string& orient_str);

    Ui::SettingsDialogClass ui;
    clany::FloraIdentSettings settings;
};

#endif // SETTINGS_DIALOG_H