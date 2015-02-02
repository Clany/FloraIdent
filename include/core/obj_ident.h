#ifndef OBJ_IDENT_H
#define OBJ_IDENT_H

#include "clany/algorithm.hpp"
#include "common.h"
#include "core/img_descriptor.h"
#include "core/svm.h"
#include "core/knn.h"

_CLANY_BEGIN
struct TrainSet
{
    using ImageSet = vector<ImageFile>;

    size_t size()     { return images.size(); }
    size_t cat_size() { return cat_name.size(); }
    bool empty() { return images.empty(); }
    void clear() {
        images.clear();
        labels.clear();
        cat_count.clear();
        cat_name.clear();
    }

    pair<int, int> subCat(int label) {
        auto idx_begin = distance(labels.begin(), find(labels, label));
        return {idx_begin, idx_begin + cat_count[label]};
    }

    ImageSet subImgs(int label) {
        auto idx_begin = distance(labels.begin(), find(labels, label));
        auto iter_begin = images.begin() + idx_begin;
        return ImageSet(iter_begin, iter_begin + cat_count[label]);
    }

    ImageSet    images;
    vector<int> labels;

    vector<int> cat_count;
    vector<string> cat_name;
};

class FloraIdent
{
public:
    void changeSettings(const array<bool, FEATURES_NUM>& features,
                        const GISTParams& gist_params,
                        bool clear_features = false);

    bool loadTrainSet(const string& dir, bool has_precompute_fts = false);
    void setTestImg(const cv::Mat& src);
    void getCandidates(array<cv::Mat, CANDIDATES_SIZE>& candidates,
                       array<string, CANDIDATES_SIZE>& cands_fn);
    auto getResult() -> string { return curr_result; };

    void genTrainFeatures();
    void genTestFeatures();
    void updateCandidates(const UserResponse& user_resp, int lambda);
    void predict();

    void clearData();
    void clearFeatures();

private:
    enum class STATUS { INITIAL, UPDATE, FINAL, COMPLETE };

    STATUS update_status = STATUS::INITIAL;
    FeatureExtractor ft_extor;

    // Train image set
    TrainSet train_set;

    // Features of train image set
    cv::Mat train_fts;
    vector<int> img_mask;
    vector<int> cat_mask;
    array<int, CANDIDATES_SIZE> cand_idx;

    vector<ml::SVM> svm_set;
    ml::KNN knn;
    ml::SVM svm;
    cv::Mat dist_mat;

    // Test image and its feature
    cv::Mat test_img;
    cv::Mat test_ft;
    string  curr_result;
};
_CLANY_END

#endif // OBJ_IDENT_H