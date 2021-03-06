#ifndef OBJ_IDENT_H
#define OBJ_IDENT_H

#include "common.h"
#include "core/img_descriptor.h"
#include "core/svm.h"
#include "core/knn.h"

_CLANY_BEGIN
struct TrainSet
{
    size_t size() { return data.size(); }
    bool empty()  { return data.empty(); }

    vector<ImageFile> data;
    vector<int> labels;
};

class FloraIdent
{
public:
    void changeSettings(const array<bool, FEATURES_NUM>& features,
                        const GISTParams& gist_params,
                        bool clear_features = false);

    bool loadTrainSet(const string& dir, bool has_precompute_fts = false);
    void setTestImg(const cv::Mat& src);
    void getCandidates(array<cv::Mat, CANDIDATES_SIZE>& candidates);

    void genTrainFeatures();
    void genTestFeatures();
    void updateCandidates(const UserResponse& user_resp, int lambda);
    string predict();

    void clearData();
    void clearFeatures();

private:
    enum class STATUS { INITIAL, FINAL };

    STATUS update_status = STATUS::INITIAL;
    FeatureExtractor ft_extor;

    // Train image set
    vector<string> cat_set;
    TrainSet train_set;

    // Features of train image set
    cv::Mat train_fts;
    vector<int> cand_idx;

    vector<ml::SVM> svm_set;
    ml::KNN classifier;
    cv::Mat dist_mat;

    // Test image and its feature
    cv::Mat test_img;
    cv::Mat test_ft;
};
_CLANY_END

#endif // OBJ_IDENT_H