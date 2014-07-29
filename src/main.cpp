#include "common.h"
#if USE_QT_VER >= 5
 #include <QtWidgets/QApplication>
#else
 #include <QtGui/QApplication>
#endif
#include "mainwindow.h"

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   MainWindow w;
   w.show();

   return app.exec();
}