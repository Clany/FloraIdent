#include "flora_ident_config.h"
#if USE_QT_VER >= 5
  #include <QtWidgets/QApplication>
#else
  #include <QtGui/QApplication>
#endif
#include <iostream>
#include "mainwindow.h"
#include "clany/file_operation.hpp"

using namespace std;
using namespace clany;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}