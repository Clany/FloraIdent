#include <QDir>
#include "flora_ident_app.h"

using namespace std;
using namespace cv;
using namespace clany;


void FloraIdentApp::setSettings(const FloraIdentSettings& settings)
{
    this->settings = settings;
    flora_ident.changeSettings(settings.used_features, settings.gist_params,
                               settings.clear_precomp_features);
}

void FloraIdentApp::setLearningRate(int learning_rate)
{
    settings.learning_rate = learning_rate;
}

bool FloraIdentApp::loadDataset(const string& dir_name)
{
    flora_ident.clearData();

    string folder_name = dir_name.substr(dir_name.rfind("/") + 1);
    if (folder_name == "Train" || folder_name == "train") {
        QDir curr_dir(dir_name.c_str());
        auto files = curr_dir.entryList(QDir::Files);

        bool has_train_fts = false;
        if (files.contains("train_features.xml") && files.contains("svm_set_0")) {
            has_train_fts = true;
        }
        return flora_ident.loadTrainSet(dir_name, has_train_fts);
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

void clany::FloraIdentApp::setSelectionRegion(const QRect& rect)
{
    setSelectionRegion(rect.left(), rect.top(), rect.width(), rect.height());
}

void FloraIdentApp::extractFeatures()
{
    flora_ident.genTrainFeatures();
    flora_ident.genTestFeatures();

    flora_ident.getCandidates(candidates);
}

void FloraIdentApp::updateCandidates(const UserResponse& user_resp)
{
    flora_ident.updateCandidates(user_resp, settings.learning_rate);
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