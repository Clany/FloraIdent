#ifndef FLORA_IDENT_H
#define FLORA_IDENT_H

#include "common.h"
#include "core/img_descriptor.h"
#include "core/svm.h"
#include "core/knn.h"

_CLANY_BEGIN
struct TrainSet
{
    size_t size() { return data.size(); }
    bool empty()  { return data.empty(); }

    vector<cv::Mat> data;
    vector<int> labels;
};

class FloraIdent
{
public:
    void changeSettings(const array<bool, FEATURES_NUM>& features,
                        const GISTParams& gist_params);

    bool loadTrainSet(const string& dir, bool has_precompute_fts = false);
    void setTestImg(const cv::Mat& src);
    void getCandidates(array<cv::Mat, CANDIDATES_SIZE>& candidates);

    void genTrainFeatures();
    void genTestFeatures();
    void updateCandidates(const UserResponse& user_resp, int lambda);
    string predict();

    void clearData();

private:
    void initDistMat();
    int updateDistMat(const cv::Mat& x1, int y1, const cv::Mat& x2, int y2, double lambda = 1);
    void nearestSPD(const cv::Mat& src, cv::Mat& dst);

    template<typename T>
    typename enable_if<is_floating_point<T>::value, T>::type
    eps(T val)
    {
        return nextafter(val, numeric_limits<T>::max()) - val;
    };

    vector<string> cat_set;
    TrainSet train_set;

    FeatureExtractor ft_extor;

    cv::Mat train_fts;
    vector<int> cand_idx;

    cv::Mat test_img;
    cv::Mat test_ft;

    vector<ml::SVM> svm_set;
    ml::KNN classifier;
    cv::Mat dist_mat;
};
_CLANY_END

#endif // FLORA_IDENT_H