#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "flora_ident_config.h"
#if USE_QT_VER >= 5
  #include <QtWidgets/QWidget>
#else
  #include <QtGui/QWidget>
#endif
#include "ui_mainwindow.h"
#include "my_widgets.h"
/*#include "flora_ident_app.h"*/

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_loadDatasetButton_clicked();

    void on_loadImageButton_clicked();

    void on_selectButton_clicked();

    void on_generateCandidatesButton_clicked();

    void on_updateCandidatesButton_clicked();

    void on_finishButton_clicked();

private:
    Ui::MainWindowClass ui;

    /*clany::FloraIdentApp flora_app;*/
};

#endif // MAINWINDOW_H
