#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "flora_ident_config.h"
#if USE_QT_VER >= 5
  #include <QtWidgets/QWidget>
#else
  #include <QtGui/QWidget>
#endif
#include "ui_mainwindow.h"
#include "flora_ident_app.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow() = default;

    void loadDataset(const QString& dir);

private:
    void showCandidates();
    void resetCandButton();
    void restoreLastSelection();
    void getUserResponses(std::array<int, clany::CANDIDATES_SIZE>& resp);

private slots:
    void on_learningRateSlider_sliderMoved(int value);

    void on_lastSelectionButton_clicked();

    void on_settingsButton_clicked();

    void on_loadDatasetButton_clicked();

    void on_loadImageButton_clicked();

    void on_selectButton_clicked();

    void on_generateCandidatesButton_clicked();

    void on_updateCandidatesButton_clicked();

    void on_finishButton_clicked();

private:
    Ui::MainWindowClass ui;

    clany::FloraIdentApp flora_app;
    std::array<QImage, clany::CANDIDATES_SIZE> candidates;
    std::array<int, clany::CANDIDATES_SIZE> last_selection;
};

#endif // MAINWINDOW_H
