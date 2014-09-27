#include <tbb/tbb.h>
#include "core/img_descriptor.h"

using namespace std;
using namespace cv;
using namespace clany;


FeatureExtractor::FeatureExtractor()
{
// #ifndef NDEBUG
//     feature_extractors.push_back(make_shared<Gist>(w, h));
// #else
//     feature_extractors.push_back(make_shared<Gist>(w, h));
//     feature_extractors.push_back(make_shared<LaplRGB>());
//     feature_extractors.push_back(make_shared<HSVHist>());
//     feature_extractors.push_back(make_shared<FourierHist>());
//     feature_extractors.push_back(make_shared<HoughHist>());
//     /*feature_extractors.push_back(make_shared<ProbRGB>());*/
// #endif
    feature_extractors.push_back(make_shared<GIST>());
    feature_extractors.push_back(make_shared<LaplRGB>());
    feature_extractors.push_back(make_shared<HSVHist>());
    feature_extractors.push_back(make_shared<FourierHist>());
    feature_extractors.push_back(make_shared<HoughHist>());
}


void FeatureExtractor::setFeatures(const FeatureFlag& flag)
{
    feature_extractors.clear();
    if (flag & FeatureFlag::GIST_DESC)    feature_extractors.push_back(make_shared<GIST>());
    if (flag & FeatureFlag::LAPL_RGB)     feature_extractors.push_back(make_shared<LaplRGB>());
    if (flag & FeatureFlag::HSV_HIST)     feature_extractors.push_back(make_shared<HSVHist>());
    if (flag & FeatureFlag::FOURIER_HIST) feature_extractors.push_back(make_shared<FourierHist>());
    if (flag & FeatureFlag::HOUGH_HIST)   feature_extractors.push_back(make_shared<HoughHist>());
}


void FeatureExtractor::setGISTParams(const GISTParams& params)
{
    GIST::setParams(params);
}


void FeatureExtractor::extract(const Mat& samples, vector<cv::Mat>& ft_vec)
{
    auto scale_ptr = scale_mat.ptr<float>();
    for (const auto& ex : feature_extractors) {
        int ft_dim = ex->size();
        cv::Mat features(1, ft_dim, CV_32FC1);

        float* data = features.ptr<float>();
        vector<float> ft;
        ex->extract(samples, ft);
        memcpy(data, ft.data(), ft_dim * sizeof(float));

        // Scale each column to [0, 1]
        float a = *scale_ptr++;
        float b = *scale_ptr++;
        features = a*features + b;

        ft_vec.push_back(features);
    }
}


void FeatureExtractor::extract(const vector<ImageFile>& samples, vector<cv::Mat>& ft_vec)
{
    scale_mat.release();
    scale_mat.create(feature_extractors.size(), 2, CV_32F);
    auto scale_ptr = scale_mat.ptr<float>();
    int ft_id = 0;
    for (const auto& ex : feature_extractors) {
        cout << "Extracting feature " << ++ft_id << "...";

        int ft_dim = ex->size();
        cv::Mat features(samples.size(), ft_dim, CV_32FC1);

        tbb::parallel_for(0, features.rows, [&samples, &features, &ex, ft_dim](int i) {
            float* data = features.ptr<float>(i);
            vector<float> ft;
            ex->extract(samples[i], ft);
            memcpy(data, ft.data(), ft_dim * sizeof(float));
        });
        cout << "done" << endl;

        // Scale value range to [0, 1]
        auto minmax_val = minmax_element(features.begin<float>(), features.end<float>());
        float a = 2.f / (*minmax_val.second - *minmax_val.first);
        float b = -(*minmax_val.second + *minmax_val.first) /
                   (*minmax_val.second - *minmax_val.first);
        features = a*features + b;
        *scale_ptr++ = a;
        *scale_ptr++ = b;

        ft_vec.push_back(features);
    }
    cout << string(50, '-') << endl;
}