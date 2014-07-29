#include "flora_ident_app.h"

using namespace std;
using namespace cv;
using namespace clany;

bool FloraIdentApp::loadDataset(const string& dir_name)
{
    flora_ident.clearData();

    string folder_name = dir_name.substr(dir_name.rfind("/") + 1);
    if (folder_name == "Train" || folder_name == "train") {
        string updir = dir_name.substr(0, dir_name.rfind("/"));
        auto files = Directory::GetListFiles(updir);

        bool has_train_fts = false;
        if (any_of(files.begin(), files.end(), [](const string& fn) { return fn.find("train_features.xml") != string::npos; }) &&
            any_of(files.begin(), files.end(), [](const string& fn) { return fn.find("svm_set_0") != string::npos; }))
        {
            has_train_fts = true;
        }
        flora_ident.loadTrainSet(dir_name, has_train_fts);

        return true;
    }
    return false;
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
    flora_ident.genTrainFeatures();
    flora_ident.genTestFeatures();

    flora_ident.getCandidates(candidates);
}

void FloraIdentApp::updateCandidates(const vector<int>& user_resp)
{
    flora_ident.updateCandidates(user_resp);
    flora_ident.getCandidates(candidates);
}

void FloraIdentApp::getCandidates(array<QImage, CANDIDATES_SIZE>& cands)
{
    matToQImage(candidates.begin(), candidates.end(), cands.begin());
}


string FloraIdentApp::getResult()
{
    return flora_ident.predict();
}

void FloraIdentApp::matToQImage(const Mat& src, QImage& dst)
{
    Mat rgb;
    cvtColor(src, rgb, CV_BGR2RGB);

    QImage tmp(rgb.data, rgb.cols, src.rows, rgb.step, QImage::Format_RGB888);
    dst = tmp.copy();
}