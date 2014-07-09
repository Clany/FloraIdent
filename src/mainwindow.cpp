#include <QFileDialog>
#include "mainwindow.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

}

MainWindow::~MainWindow()
{

}


//////////////////////////////////////////////////////////////////////////
// Callback functions
void MainWindow::on_loadDatasetButton_clicked()
{
    // Todo
    ui.outputPanel->append("Load dataset button clicked");

    ui.loadImageButton->setDisabled(false);
}

void MainWindow::on_loadImageButton_clicked()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("Open Image"), "../data",
                                                    tr("Image Files (*.png *.jpg *.bmp)"));
    if (!fn.isEmpty()) {
        /*flora_app.loadQueryImg(fn.toStdString());*/
        ui.queryImageLabel->setImage(QImage(fn));
        ui.queryImageLabel->setDisabled(false);
        ui.selectButton->setDisabled(false);
    }
}

void MainWindow::on_selectButton_clicked()
{
    QRect selected_region;
    if (ui.queryImageLabel->getSelectionArea(selected_region)) {
        float ratio = 1 / ui.queryImageLabel->getRatio();
//         flora_app.setSelectionRegion(ratio * selected_region.left(),
//                                      ratio * selected_region.top(),
//                                      ratio * selected_region.width(),
//                                      ratio * selected_region.height());
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
    // Todo
    ui.outputPanel->append("Generate Candidates button clicked");

    ui.updateCandidatesButton->setDisabled(false);
    ui.finishButton->setDisabled(false);
}

void MainWindow::on_updateCandidatesButton_clicked()
{
    // Todo
    ui.outputPanel->append("Update Candidates button clicked");
}

void MainWindow::on_finishButton_clicked()
{
//    QApplication::exit();
}