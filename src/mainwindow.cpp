#include <QFileDialog>
#include <QButtonGroup>
#include "mainwindow.h"

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

MainWindow::~MainWindow()
{}


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
}

//////////////////////////////////////////////////////////////////////////
// Callback functions
void MainWindow::on_loadDatasetButton_clicked()
{
    QString fn = QFileDialog::getExistingDirectory(this, tr("Open Dataset"));

    if (!fn.isEmpty()) {
        ui.outputPanel->append("Loading dataset...");
        ui.outputPanel->repaint();
        flora_app.loadDataset(fn.toStdString());
        ui.outputPanel->moveCursor(QTextCursor::End);
        ui.outputPanel->insertPlainText("done.");

        ui.loadImageButton->setDisabled(false);
        return;
    }
//    ui.outputPanel->append("No dataset imported");
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
        ui.queryImageLabel->setDisabled(false);
        ui.selectButton->setDisabled(false);
        return;
    }
//    ui.outputPanel->append("No image selected");
}

void MainWindow::on_selectButton_clicked()
{
    QRect selected_region;
    if (ui.queryImageLabel->getSelectionArea(selected_region)) {
        float ratio = 1 / ui.queryImageLabel->getRatio();
        flora_app.setSelectionRegion(ratio * selected_region.left(),
                                     ratio * selected_region.top(),
                                     ratio * selected_region.width(),
                                     ratio * selected_region.height());
        string region = string("Selected image region: [x: ") +
                        to_string(selected_region.left()) + ", y: " +
                        to_string(selected_region.top()) + ", w: " +
                        to_string(selected_region.width()) + ", h: " +
                        to_string(selected_region.height()) + "]";
        ui.outputPanel->append(region.c_str());
        ui.generateCandidatesButton->setDisabled(false);
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

    ui.candidateGroup->setDisabled(false);
    ui.updateCandidatesButton->setDisabled(false);
    ui.finishButton->setDisabled(false);
}

void MainWindow::on_updateCandidatesButton_clicked()
{
    vector<int> user_resp(6, 0);
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

    flora_app.updateCandidates(user_resp);

    flora_app.getCandidates(candidates);
    showCandidates();
}

void MainWindow::on_finishButton_clicked()
{
    string result = flora_app.getResult();
    ui.outputPanel->append((string("Final result: ") + result).c_str());

    ui.candidateGroup->setDisabled(true);
    ui.generateCandidatesButton->setDisabled(true);
    ui.updateCandidatesButton->setDisabled(true);
    ui.finishButton->setDisabled(true);
}