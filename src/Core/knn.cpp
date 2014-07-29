#include <map>
#include "core/knn.h"

using namespace std;
using namespace cv;
using namespace clany;
using namespace ml;

bool KNN::train(const cv::Mat& trainData, const cv::Mat& responses,
                const cv::Mat& sampleIdx, bool isRegression, int maxK, bool updateBase)
{
    train_data = trainData;
    labels = responses;

    Mat train_mat;
    trainData.convertTo(train_mat, CV_32FC1);
    return cv::KNearest::train(train_mat, responses, sampleIdx, isRegression, maxK, updateBase);
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

    int cat = find_nearest(sample, k, nullptr, idx_ptr, resp_ptr, dist_ptr);

    if (idx_ptr) {
        int* p = neighbor_idx.getMat().ptr<int>();
        for (const auto& neighbor : neighbor_vec) {
            *p = (neighbor - *samples->data.fl) / sample.cols;
            ++p;
        }
    }

    return cat;
}

int KNN::predict(const Mat& sample, int k, OutputArray neighbor_idx, DistFunc dist_func)
{
    int train_sz = train_data.rows;
    Mat dist(1, train_sz, CV_64FC1);

    double* dist_ptr = dist.ptr<double>();
    for (int i = 0; i < train_sz; ++i) {
        *dist_ptr = dist_func(train_data.row(i), sample);
        ++dist_ptr;
    }

    sortIdx(dist, neighbor_idx, CV_SORT_EVERY_ROW | CV_SORT_ASCENDING);
    Mat idx = neighbor_idx.getMat();

    // map: <label, count>
    int* idx_ptr   = idx.ptr<int>();
    int* label_ptr = labels.ptr<int>();
    map<int, int> neighbor_labels;
    for (int i = 0; i < k; ++i) {
        int label = label_ptr[idx_ptr[i]];
        if (neighbor_labels.find(label) == neighbor_labels.end()) {
            neighbor_labels.insert({label, 1});
        } else {
            ++neighbor_labels[label];
        }
    }
    vector<pair<int, int>> sorted(neighbor_labels.begin(), neighbor_labels.end());
    sort(sorted.begin(), sorted.end(), [](const pair<int, int>& p1, const pair<int, int>& p2) {
        return p1.second > p2.second;
    });

    return sorted.begin()->first;
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