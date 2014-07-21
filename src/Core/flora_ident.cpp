#include "core/flora_ident.h"

using namespace std;
using namespace cv;
using namespace clany;

void FloraIdent::loadTrainSet(const string& dir)
{
    vector<string> loc_vec = Directory::GetListFolders(dir);
    int label = 0;
    for (const auto& loc : loc_vec) {
        vector<string> cat_vec = Directory::GetListFolders(loc);
        for (const auto& cat : cat_vec) {
            vector<string> files = Directory::GetListFiles(cat);
            for (const auto& fn : files) {
                train_set.data.push_back(imread(fn));
                train_set.labels.push_back(label);
            }
            cat_set.push_back(cat.substr(cat.rfind("_") + 1));
            ++label;
        }
    }
}

void FloraIdent::setTestImg(const Mat& src)
{
    test_img = src.clone();
}

void FloraIdent::genTrainFeatures()
{
    vector<Mat> train_ft_vec;
    FeatureExtractor::extract(train_set.data, train_ft_vec);

    size_t train_sz = train_set.size();
    size_t cat_sz = cat_set.size();
    train_fts.create(train_sz, cat_sz*train_ft_vec.size(), CV_64FC1);
    for (int i = 0; i < train_ft_vec.size(); ++i) {
        train_ft_vec[i].convertTo(train_ft_vec[i], CV_64F);

        ml::SVM svm;
        svm.train(train_ft_vec[i], train_set.labels);
        for (int j = 0; j < train_sz; ++j) {
            ml::SVMNode node;
            node.dim = train_ft_vec[i].cols;
            node.values = train_ft_vec[i].ptr<double>(j);
            svm.predict(node, train_fts.ptr<double>(j) +i*cat_sz, true);
        }
        svm_set.push_back(svm);
    }
}

void FloraIdent::genTestFeatures()
{
    vector<Mat> test_ft_vec;
    FeatureExtractor::extract({test_img}, test_ft_vec);

    size_t cat_sz = cat_set.size();
    test_ft.create(1, cat_sz*test_ft_vec.size(), CV_64FC1);
    for (int i = 0; i < svm_set.size(); ++i) {
        test_ft_vec[i].convertTo(test_ft_vec[i], CV_64F);

        ml::SVMNode node;
        node.dim = test_ft_vec[i].cols;
        node.values = test_ft_vec[i].ptr<double>();
        svm_set[i].predict(node, test_ft.ptr<double>() + i*cat_sz, true);
    }

    Mat ft_mat;
    train_fts.convertTo(ft_mat, CV_32F);
    Mat label_mat(train_set.size(), 1, CV_32SC1, train_set.labels.data());
    classifier.train(ft_mat, label_mat);
}


void FloraIdent::updateCandidates()
{

}

string FloraIdent::predict()
{
    Mat ft_mat, result;
    test_ft.convertTo(ft_mat, CV_32F);
    int idx = cvRound(classifier.find_nearest(ft_mat, 10, result, Mat(), Mat()));

    return cat_set[idx];
}