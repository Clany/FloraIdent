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


void FeatureExtractor::extract(const Mat& src, vector<vector<float>>& features)
{
    vector<float> ft;

    for (const auto& ex : feature_extractors) {
        ex->extract(src, ft);
        features.push_back(move(ft));
    }
}


void FeatureExtractor::mergeFeature(const vector<vector<vector<float>>>& features_src,
                                    vector<vector<float>>& features_dst)
{
    features_dst.clear();
    features_dst.resize(features_src.size());

    int sz = features_dst.size();
    for (int i = 0; i < sz; ++i) {
        const auto& ft_vec = features_src[i];
        auto& ft_dst = features_dst[i];

        int ft_sz = accumulate(ft_vec.begin(), ft_vec.end(), 0, [](int init, const vector<float>& ft) {
            return init + ft.size();
        });
        ft_dst.reserve(ft_sz);

        for (const auto& ft : ft_vec) {
            ft_dst.insert(ft_dst.end(), ft.begin(), ft.end());
        }
    }
}