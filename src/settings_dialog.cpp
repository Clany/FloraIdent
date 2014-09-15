#include <stdexcept>
#include <QMessageBox>

#include "settings_dialog.h"
#include "flora_ident_app.h"

using namespace std;
using namespace clany;


SettingsDialog::SettingsDialog(QWidget* parent, Qt::WindowFlags flag)
    : QDialog(parent, flag)
{
    ui.setupUi(this);
}

SettingsDialog::SettingsDialog(QWidget* parent, Qt::WindowFlags flag,
                               const FloraIdentSettings& init_settings)
    : SettingsDialog(parent, flag)
{
    init(init_settings);
}

void SettingsDialog::init(const FloraIdentSettings& init_settings)
{
    settings = init_settings;

    ui.GISTCheckBox->setChecked(settings.used_features[0]);
    ui.LaplRGBCheckBox->setChecked(settings.used_features[1]);
    ui.HSVHistCheckBox->setChecked(settings.used_features[2]);
    ui.FourierHistCheckBox->setChecked(settings.used_features[3]);
    ui.HoughHistCheckBox->setChecked(settings.used_features[4]);

    ui.ColorGISTCheckBox->setChecked(settings.gist_params.use_color);
    ui.GISTWidthSpinBox->setValue(settings.gist_params.width);
    ui.GISTHeightSpinBox->setValue(settings.gist_params.height);
    ui.GISTBlockSpinBox->setValue(settings.gist_params.blocks);
    ui.GISTScaleSpinBox->setValue(settings.gist_params.scale);

    string orient_str;
    parseOrientaion(settings.gist_params.orients, orient_str);
    ui.GISTOrientLineEdit->setText(orient_str.c_str());
}

FloraIdentSettings SettingsDialog::getSettings()
{
    FloraIdentSettings new_settings;

    new_settings.used_features[0] = ui.GISTCheckBox->isChecked();
    new_settings.used_features[1] = ui.LaplRGBCheckBox->isChecked();
    new_settings.used_features[2] = ui.HSVHistCheckBox->isChecked();
    new_settings.used_features[3] = ui.FourierHistCheckBox->isChecked();
    new_settings.used_features[4] = ui.HoughHistCheckBox->isChecked();

    new_settings.gist_params.use_color = ui.ColorGISTCheckBox->isChecked();
    new_settings.gist_params.width     = ui.GISTWidthSpinBox->value();
    new_settings.gist_params.height    = ui.GISTHeightSpinBox->value();
    new_settings.gist_params.blocks    = ui.GISTBlockSpinBox->value();
    new_settings.gist_params.scale     = ui.GISTScaleSpinBox->value();
    parseOrientaion(new_settings.gist_params.orients);

    if (!equal(new_settings.used_features.begin(), new_settings.used_features.end(),
               settings.used_features.begin())) {
        if (QMessageBox::Yes == QMessageBox::question(this, "Feature list changed",
            "You've changed the feature list, clear precomputed features?")) {
            new_settings.clear_precomp_features = true;
        }
    }

    settings = new_settings;
    return settings;
}


//////////////////////////////////////////////////////////////////////////
// Public slots
void SettingsDialog::accept()
{
    vector<int> orients;

    if (!parseOrientaion(orients)) {
        QMessageBox::critical(this, "Invalid settings", "Invalid orienations! example: \"8, 8, 8, 8\"");
        return;
    }
    QDialog::accept();
}

//////////////////////////////////////////////////////////////////////////
// Private functions
bool SettingsDialog::parseOrientaion(vector<int>& orients)
{
    string orient_str = ui.GISTOrientLineEdit->text().toStdString();
    stringstream ss(orient_str);

    try {
        string tmp;
        orients.clear();
        while (getline(ss, tmp, ',')) {
            orients.push_back(stoi(tmp));   // May throw
        }

        if (orients.size() != ui.GISTScaleSpinBox->value()) {
            throw logic_error("Invalid orienations!");
        }
    } catch (const logic_error& err) {
        cerr << err.what() << endl;
        return false;
    } catch (...) {
        cerr << "Unknown exception!" << endl;
        return false;
    }

    return true;
}

void SettingsDialog::parseOrientaion(const vector<int>& orients, string& orient_str)
{
    stringstream ss;
    for (const auto& orient : orients) {
        ss << orient << ",";
    }
    ss >> orient_str;
    orient_str.erase(orient_str.end() - 1);
}