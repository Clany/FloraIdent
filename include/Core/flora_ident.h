#ifndef FLORA_IDENT_H
#define FLORA_IDENT_H

#include "common.h"
#include "core/img_descriptor.h"
#include "core/svm.h"

_CLANY_BEGIN
struct TrainSet
{
    size_t size() { return data.size(); }
    vector<cv::Mat> data;
    vector<int> labels;
};

class FloraIdent
{
public:
    void loadTrainSet(const string& dir);
    void setTestImg(const cv::Mat& src);

    void genTrainFeatures();
    void genTestFeatures();
    void updateCandidates();
    string predict();

private:
    vector<string> cat_set;
    TrainSet train_set;
    cv::Mat train_fts;

    cv::Mat test_img;
    cv::Mat test_ft;

    vector<ml::SVM> svm_set;
    cv::KNearest classifier;
};
_CLANY_END

#endif // FLORA_IDENT_H