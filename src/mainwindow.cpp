#include <QFileDialog>
#include <QButtonGroup>
#include "mainwindow.h"
#include "my_widgets.h"
#include "settings_dialog.h"
#include "flora_ident_app.h"

using namespace std;
using namespace clany;

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    ui.candidateGroup->layout()->setAlignment(ui.updateCandidatesButton, Qt::AlignCenter);

    auto addRadioButtonGroup = [this](QRadioButton* similar,
                                      QRadioButton* dissimilar,
                                      QRadioButton* unknown) {
        QButtonGroup* bg = new QButtonGroup(this);
        bg->addButton(similar);
        bg->addButton(dissimilar);
        bg->addButton(unknown);
        unknown->setChecked(true);
    };

    addRadioButtonGroup(ui.candSimilar_1, ui.candDissimilar_1, ui.candUnknown_1);
    addRadioButtonGroup(ui.candSimilar_2, ui.candDissimilar_2, ui.candUnknown_2);
    addRadioButtonGroup(ui.candSimilar_3, ui.candDissimilar_3, ui.candUnknown_3);
    addRadioButtonGroup(ui.candSimilar_4, ui.candDissimilar_4, ui.candUnknown_4);
    addRadioButtonGroup(ui.candSimilar_5, ui.candDissimilar_5, ui.candUnknown_5);
    addRadioButtonGroup(ui.candSimilar_6, ui.candDissimilar_6, ui.candUnknown_6);
}


//////////////////////////////////////////////////////////////////////////
// Public functions
void MainWindow::loadDataset(const QString& dir)
{
    if (!dir.isEmpty()) {
        ui.outputPanel->append("Loading dataset...");
        ui.outputPanel->repaint();
        if (flora_app.loadDataset(QDir::cleanPath(dir).toStdString())) {
            ui.outputPanel->moveCursor(QTextCursor::End);
            ui.outputPanel->insertPlainText("done.");

            ui.loadImageButton->setEnabled(true);
        } else {
            ui.outputPanel->moveCursor(QTextCursor::End);
            ui.outputPanel->insertPlainText("error, dataset is not correctly loaded!");
        }
    }
}


//////////////////////////////////////////////////////////////////////////
// Private functions
void MainWindow::showCandidates()
{
    ui.candidateImageLabel_1->setImage(candidates[0]);
    ui.candidateImageLabel_2->setImage(candidates[1]);
    ui.candidateImageLabel_3->setImage(candidates[2]);
    ui.candidateImageLabel_4->setImage(candidates[3]);
    ui.candidateImageLabel_5->setImage(candidates[4]);
    ui.candidateImageLabel_6->setImage(candidates[5]);
    ui.horizontalLayout_7->setAlignment(ui.candidateImageLabel_1, Qt::AlignCenter);
    ui.horizontalLayout_7->setAlignment(ui.candidateImageLabel_2, Qt::AlignCenter);
    ui.horizontalLayout_7->setAlignment(ui.candidateImageLabel_3, Qt::AlignCenter);
    ui.horizontalLayout_9->setAlignment(ui.candidateImageLabel_4, Qt::AlignCenter);
    ui.horizontalLayout_9->setAlignment(ui.candidateImageLabel_5, Qt::AlignCenter);
    ui.horizontalLayout_9->setAlignment(ui.candidateImageLabel_6, Qt::AlignCenter);

    resetCandButton();
}

void MainWindow::resetCandButton()
{
    ui.candUnknown_1->setChecked(true);
    ui.candUnknown_2->setChecked(true);
    ui.candUnknown_3->setChecked(true);
    ui.candUnknown_4->setChecked(true);
    ui.candUnknown_5->setChecked(true);
    ui.candUnknown_6->setChecked(true);
}

void MainWindow::restoreLastSelection()
{
    auto setRadioButton = [](int id, QRadioButton* similar,
                                     QRadioButton* dissimilar,
                                     QRadioButton* unknown) {
        if (id ==  1) similar->setChecked(true);
        if (id == -1) dissimilar->setChecked(true);
        if (id ==  0) unknown->setChecked(true);
    };

    setRadioButton(last_selection[0], ui.candSimilar_1, ui.candDissimilar_1, ui.candUnknown_1);
    setRadioButton(last_selection[1], ui.candSimilar_2, ui.candDissimilar_2, ui.candUnknown_2);
    setRadioButton(last_selection[2], ui.candSimilar_3, ui.candDissimilar_3, ui.candUnknown_3);
    setRadioButton(last_selection[3], ui.candSimilar_4, ui.candDissimilar_4, ui.candUnknown_4);
    setRadioButton(last_selection[4], ui.candSimilar_5, ui.candDissimilar_5, ui.candUnknown_5);
    setRadioButton(last_selection[5], ui.candSimilar_6, ui.candDissimilar_6, ui.candUnknown_6);
}

void MainWindow::getUserResponses(UserResponse& user_resp)
{
    user_resp.fill(0);

    if (!ui.candUnknown_1->isChecked()) {
        user_resp[0] = ui.candSimilar_1->isChecked() ? 1 : -1;
    }
    if (!ui.candUnknown_2->isChecked()) {
        user_resp[1] = ui.candSimilar_2->isChecked() ? 1 : -1;
    }
    if (!ui.candUnknown_3->isChecked()) {
        user_resp[2] = ui.candSimilar_3->isChecked() ? 1 : -1;
    }
    if (!ui.candUnknown_4->isChecked()) {
        user_resp[3] = ui.candSimilar_4->isChecked() ? 1 : -1;
    }
    if (!ui.candUnknown_5->isChecked()) {
        user_resp[4] = ui.candSimilar_5->isChecked() ? 1 : -1;
    }
    if (!ui.candUnknown_6->isChecked()) {
        user_resp[5] = ui.candSimilar_6->isChecked() ? 1 : -1;
    }
}


//////////////////////////////////////////////////////////////////////////
// Callback functions
void MainWindow::on_learningRateSlider_sliderMoved(int value)
{
    flora_app.setLearningRate(value);
    auto new_rate = string("Learning rate: ") + to_string(value);
    ui.learningRateLabel->setText(new_rate.c_str());
}

void MainWindow::on_lastSelectionButton_clicked()
{
    restoreLastSelection();
}

void MainWindow::on_settingsButton_clicked()
{
    SettingsDialog dialog(this, Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    dialog.init(flora_app.getSettings());
    if (QDialog::Accepted == dialog.exec()) {
        flora_app.setSettings(dialog.getSettings());
    }
}

void MainWindow::on_loadDatasetButton_clicked()
{
    QString dir_name = QFileDialog::getExistingDirectory(this, tr("Open Dataset"));
    loadDataset(dir_name);
}

void MainWindow::on_loadImageButton_clicked()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("Open Image"), "../data",
                                                    tr("Image Files (*.png *.jpg *.bmp)"));
    if (!fn.isEmpty()) {
        flora_app.loadQueryImg(fn.toStdString());

        ui.queryImageLabel->setImage(QImage(fn));
        ui.queryGroup->layout()->setAlignment(ui.queryImageLabel, Qt::AlignCenter);
        ui.queryImageLabel->clearSelection();
        ui.queryImageLabel->setEnabled(true);
        ui.selectButton->setEnabled(true);
    }
}

void MainWindow::on_selectButton_clicked()
{
    QRect selected_region;
    string region_string;
    if (ui.queryImageLabel->getSelectionArea(selected_region, region_string, true)) {
        flora_app.setSelectionRegion(selected_region);

        ui.outputPanel->append(region_string.c_str());
        ui.generateCandidatesButton->setEnabled(true);
    } else {
        ui.outputPanel->append("No region selected");
    }
}

void MainWindow::on_generateCandidatesButton_clicked()
{
    ui.outputPanel->append("Generating features...");
    ui.outputPanel->repaint();

    flora_app.extractFeatures();

    ui.outputPanel->moveCursor(QTextCursor::End);
    ui.outputPanel->insertPlainText("done.");

    flora_app.getCandidates(candidates);
    showCandidates();

    ui.candidateGroup->setEnabled(true);
    ui.updateCandidatesButton->setEnabled(true);
    ui.finishButton->setEnabled(true);
    ui.settingsButton->setDisabled(true);
}

void MainWindow::on_updateCandidatesButton_clicked()
{
    getUserResponses(last_selection);

//     flora_app.updateCandidates(last_selection);
//     flora_app.getCandidates(candidates);
//     showCandidates();
    resetCandButton();
}

void MainWindow::on_finishButton_clicked()
{
    string result = flora_app.getResult();
    ui.outputPanel->append((string("Final result: ") + result).c_str());

    ui.candidateGroup->setDisabled(true);
    ui.generateCandidatesButton->setDisabled(true);
    ui.updateCandidatesButton->setDisabled(true);
    ui.finishButton->setDisabled(true);
    ui.settingsButton->setEnabled(true);
}