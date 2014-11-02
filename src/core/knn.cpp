#include <map>
#include <tbb/tbb.h>
#include <clany/algorithm.hpp>
#include "core/knn.h"

using namespace std;
using namespace cv;
using namespace tbb;
using namespace cls;
using namespace ml;

bool KNN::train(const cv::Mat& trainData, const cv::Mat& responses,
                const cv::Mat& sampleIdx, bool isRegression,
                int maxK, bool updateBase)
{
    train_data = trainData;
    labels = responses;

    Mat train_mat;
    trainData.convertTo(train_mat, CV_32FC1);
    return cv::KNearest::train(train_mat, responses, sampleIdx,
                               isRegression, maxK, updateBase);
}

int KNN::predict(const Mat& sample, int k, OutputArray neighbor_idx,
                 OutputArray _neighbor_resp, OutputArray _dist)
{
    const float** idx_ptr = nullptr;
    Mat* resp_ptr = nullptr;
    Mat* dist_ptr = nullptr;
    vector<const float*> neighbor_vec;
    Mat resp_mat, dist_mat;

    if (neighbor_idx.needed()) {
        neighbor_idx.create(1, k, CV_32SC1);
        neighbor_vec.resize(k);
        idx_ptr = neighbor_vec.data();
    }
    if (_neighbor_resp.needed()) {
        _neighbor_resp.create(1, k, CV_32FC1);
        resp_mat = _neighbor_resp.getMat();
        resp_ptr = &resp_mat;
    }
    if (_dist.needed()) {
        _dist.create(1, k, CV_32FC1);
        dist_mat = _dist.getMat();
        dist_ptr = &dist_mat;
    }

    int cat = static_cast<int>(find_nearest(sample, k, nullptr, idx_ptr,
                                            resp_ptr, dist_ptr));

    if (idx_ptr) {
        int* p = neighbor_idx.getMat().ptr<int>();
        for (const auto& neighbor : neighbor_vec) {
            *p = (neighbor - *samples->data.fl) / sample.cols;
            ++p;
        }
    }

    return cat;
}

int KNN::predict(const Mat& sample, int k, OutputArray neighbor_idx,
                 DistFunc dist_func)
{
    vector<int> guesses;
    predict(sample, k, guesses, 1, neighbor_idx, dist_func);

    return guesses[0];
}

void KNN::predict(const Mat& sample, int k, vector<int>& guesses,
                  int guess_num, OutputArray neighbor_idx, DistFunc dist_func)
{
    int train_sz = train_data.rows;
    Mat dist(1, train_sz, CV_64FC1);

    double* dist_ptr = dist.ptr<double>();
    parallel_for(0, train_sz, [&](int i) {
        dist_ptr[i] = dist_func(train_data.row(i), sample);
    });


    sortIdx(dist, neighbor_idx, CV_SORT_EVERY_ROW | CV_SORT_ASCENDING);
    Mat idx = neighbor_idx.getMat();

    // map: <label, count>
    int* idx_ptr = idx.ptr<int>();
    int* label_ptr = labels.ptr<int>();
    map<int, int> neighbor_labels;
    for (int i = 0; i < k; ++i) {
        int label = label_ptr[idx_ptr[i]];
        ++neighbor_labels[label];
    }

    vector<pair<int, int>> sorted;
    copy(neighbor_labels, sorted);
    sort(sorted, [](const pair<int, int>& p1, const pair<int, int>& p2) {
        return p1.second > p2.second;
    });

    if (sorted.size() < (size_t)guess_num) guess_num = sorted.size();
    sorted.resize(guess_num);

    auto get_label = [](const pair<int, int>& p) { return p.first; };
    transform(sorted, guesses, get_label);
}

double KNN::euclideanDist(InputArray _a1, InputArray _a2)
{
    Mat a1 = _a1.getMat();
    Mat a2 = _a2.getMat();

    CV_Assert((a1.size() == a2.size()) &&
              (a1.rows == 1 || a1.cols == 1) &&
              (a2.rows == 1 || a2.cols == 1));

    auto delta = a1 - a2;
    return sqrt(delta.dot(delta));
}