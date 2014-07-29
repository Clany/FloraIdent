#ifndef KNN_H
#define KNN_H

#include <opencv2/ml/ml.hpp>
#include "common.h"

_CLANY_BEGIN
namespace ml {
using namespace placeholders;

class KNN : public cv::KNearest
{
public:
    /*using CvKNearest::CvKNearest;*/   // Inheriting constructor

    using DistFunc = function<double(cv::InputArray, cv::InputArray)>;

    bool train(const cv::Mat& trainData, const cv::Mat& responses,
               const cv::Mat& sampleIdx = cv::Mat(), bool isRegression = false,
               int maxK = 32, bool updateBase = false) override;

    int predict(const cv::Mat& sample, int k, cv::OutputArray neighbor_idx = cv::noArray(),
                cv::OutputArray neighbor_resp = cv::noArray(), cv::OutputArray dist = cv::noArray());

    int predict(const cv::Mat& sample, int k, cv::OutputArray neighbor_idx,
                DistFunc dist_func = &KNN::euclideanDist);

    static double euclideanDist(cv::InputArray a1, cv::InputArray a2);

private:
    cv::Mat train_data;
    cv::Mat labels;
};

} // End namespace ml
_CLANY_END

#endif // KNN_H