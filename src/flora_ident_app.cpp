#include "flora_ident_app.h"

using namespace std;
using namespace cv;
using namespace clany;

void FloraIdentApp::loadDataset(const string& dir_name)
{
    flora_ident.loadTrainSet(dir_name);
}

void FloraIdentApp::loadQueryImg(const string& file_name)
{
    query_img = imread(file_name);
}

void FloraIdentApp::setSelectionRegion(int x, int y, int width, int height)
{
    flora_ident.setTestImg(query_img(Rect(x, y, width, height)));
}

void FloraIdentApp::extractFeatures()
{
    if (!has_train_fts) {
        flora_ident.genTrainFeatures();
        has_train_fts = true;
    }

    flora_ident.genTestFeatures();
}

void FloraIdentApp::updateCandidates(const vector<int> user_resp)
{
    flora_ident.updateCandidates();
}

string FloraIdentApp::getResult()
{
    return flora_ident.predict();
}