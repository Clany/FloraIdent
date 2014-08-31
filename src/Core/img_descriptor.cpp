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
    feature_extractors.push_back(make_shared<Gist>(w, h));
    feature_extractors.push_back(make_shared<LaplRGB>());
    feature_extractors.push_back(make_shared<HSVHist>());
    feature_extractors.push_back(make_shared<FourierHist>());
    feature_extractors.push_back(make_shared<HoughHist>());
}


void FeatureExtractor::extract(const Mat& src, vector<vector<float>>& features)
{
    vector<float> ft;

    for (const auto& ex : feature_extractors) {
        ex->extract(src, ft);
        features.push_back(move(ft));
    }
}


void FeatureExtractor::extract(const vector<Mat>& samples, vector<Mat>& ft_vec)
{
    int ft_id = 0;
    for (const auto& ex : feature_extractors) {
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