#include "flora_ident_app.h"

using namespace std;
using namespace cv;
using namespace clany;

void FloraIdentApp::loadDataset(const string& file_name)
{

}

void FloraIdentApp::loadQueryImg(const string& file_name)
{
    query_img = imread(file_name);
}

void FloraIdentApp::setSelectionRegion(int x, int y, int width, int height)
{
    cropped_img = query_img(Rect(x, y, width, height)).clone();
}