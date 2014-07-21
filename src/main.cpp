#include "common.h"
#if USE_QT_VER >= 5
  #include <QtWidgets/QApplication>
#else
  #include <QtGui/QApplication>
#endif
#include "mainwindow.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}