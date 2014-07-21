#include <QFileDialog>
#include "mainwindow.h"

using namespace std;
using namespace clany;

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

}

MainWindow::~MainWindow()
{

}

void MainWindow::showCandidates()
{
    ui.candidateImageLabel_1->setImage(candidates[0]);
    ui.candidateImageLabel_2->setImage(candidates[1]);
    ui.candidateImageLabel_3->setImage(candidates[2]);
    ui.candidateImageLabel_4->setImage(candidates[3]);
    ui.candidateImageLabel_5->setImage(candidates[4]);
    ui.candidateImageLabel_6->setImage(candidates[5]);
}

void MainWindow::matToQImage(const cv::Mat& src, QImage& dst)
{
    cv::Mat rgb;
    cvtColor(src, rgb, CV_BGR2RGB);

    QImage tmp(rgb.data, rgb.cols, src.rows, rgb.step, QImage::Format_RGB888);
    dst = tmp.copy();
}

template<typename InputIterator, typename OutputIterator>
void MainWindow::matToQImage(InputIterator first, InputIterator last, OutputIterator result)
{
    while (first != last) {
        matToQImage(*first, *result);
        ++first;
        ++result;
    }
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

//     const auto& cands = flora_app.getCandidates();
//     matToQImage(cands.begin(), cands.end(), candidates.begin());
//     showCandidates();

    ui.updateCandidatesButton->setDisabled(false);
    ui.finishButton->setDisabled(false);
}

void MainWindow::on_updateCandidatesButton_clicked()
{
    vector<int> user_resp(6, 0);
    if (!ui.candUnknown_1->isChecked()) {
        user_resp[0] = ui.canSimilar_1->isChecked() ? 1 : -1;
    }
    if (!ui.candUnknown_2->isChecked()) {
        user_resp[1] = ui.canSimilar_2->isChecked() ? 1 : -1;
    }
    if (!ui.candUnknown_3->isChecked()) {
        user_resp[2] = ui.canSimilar_3->isChecked() ? 1 : -1;
    }
    if (!ui.candUnknown_4->isChecked()) {
        user_resp[3] = ui.canSimilar_4->isChecked() ? 1 : -1;
    }
    if (!ui.candUnknown_5->isChecked()) {
        user_resp[4] = ui.canSimilar_5->isChecked() ? 1 : -1;
    }
    if (!ui.candUnknown_6->isChecked()) {
        user_resp[5] = ui.canSimilar_6->isChecked() ? 1 : -1;
    }

    flora_app.updateCandidates(user_resp);

//     const auto& cands = flora_app.getCandidates();
//     matToQImage(cands.begin(), cands.end(), candidates.begin());
//     showCandidates();
}

void MainWindow::on_finishButton_clicked()
{
    string result = flora_app.getResult();
    ui.outputPanel->append((string("Final result: ") + result).c_str());

    ui.generateCandidatesButton->setDisabled(true);
    ui.updateCandidatesButton->setDisabled(true);
    ui.finishButton->setDisabled(true);
}