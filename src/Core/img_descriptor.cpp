#include "core/img_descriptor.h"

using namespace std;
using namespace cv;
using namespace clany;


namespace  {
    constexpr int GIST_WIDTH = 500;
} // unamed namespace

int FeatureExtractor::w = 0;
int FeatureExtractor::h = 0;

FeatureExtractor::FeatureExtractor(int width, int height)
{

// #ifndef NDEBUG
//     feature_extractors.push_back(make_shared<HSVHist>());
// #else
//     /*feature_extractors.push_back(make_shared<ProbRGB>());*/
//     feature_extractors.push_back(make_shared<LaplRGB>());
//     feature_extractors.push_back(make_shared<HSVHist>());
//     feature_extractors.push_back(make_shared<FourierHist>());
//     feature_extractors.push_back(make_shared<HoughHist>());
// #endif
    feature_extractors.push_back(make_shared<Gist>(width, height/*some params to resize the image*/));
    feature_extractors.push_back(make_shared<LaplRGB>());
    feature_extractors.push_back(make_shared<HSVHist>());
    feature_extractors.push_back(make_shared<FourierHist>());
    feature_extractors.push_back(make_shared<HoughHist>());
}


void FeatureExtractor::extract(const Mat& src, vector<vector<float>>& features)
{
    vector<float> ft;

    const auto& extractors = instance().feature_extractors;
    for (const auto& extractor : extractors) {
        extractor->extract(src, ft);
        features.push_back(move(ft));
    }
}


void FeatureExtractor::extract(const vector<Mat>& samples, vector<Mat>& ft_vec)
{
    const auto& extractors = instance().feature_extractors;
    int ft_id = 0;
    for (const auto& ex : extractors) {
        cout << "Extracting feature " << ++ft_id << endl;

        int ft_dim = ex->size();
        Mat features(samples.size(), ft_dim, CV_32FC1);

        int sz = samples.size();
#pragma omp parallel for
        for (int i = 0; i < sz; ++i) {
            float* data = features.ptr<float>(i);
            vector<float> ft;
            ex->extract(samples[i], ft);
            memcpy(data, ft.data(), ft_dim * sizeof(float));
        }
        ft_vec.push_back(features);
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