#include "common.h"
#if USE_QT_VER >= 5
 #include <QtWidgets/QApplication>
#else
 #include <QtGui/QApplication>
#endif
#include "mainwindow.h"

using namespace std;
using namespace cv;
using namespace clany;

int main(int argc, char *argv[])
{
   QApplication app(argc, argv);
   MainWindow w;
   w.show();

   if (argc != 1) {
       w.loadDataset(argv[1]);
   }

   return app.exec();
}