#ifndef IMG_DESCRIPTOR_H
#define IMG_DESCRIPTOR_H

#include "common.h"
#include "gist.h"
#include "color_hist.h"
#include "fourier_hist.h"
#include "hough_hist.h"


_CLANY_BEGIN
constexpr int CANDIDATES_SIZE = 6;
constexpr int FEATURES_NUM = 5;
using UserResponse = array<int, CANDIDATES_SIZE>;

class ImageFile {
public:
    explicit ImageFile(const string& file_name) : fn(file_name) {}

    operator cv::Mat() const { return cv::imread(fn); }

private:
    string fn;
};

class FeatureExtractor {
public:
    enum FeatureFlag { GIST_DESC = 1, LAPL_RGB = 2, HSV_HIST = 4,
                       FOURIER_HIST = 8, HOUGH_HIST = 16 };

    FeatureExtractor();

    FeatureExtractor(const FeatureExtractor&) = delete;
    FeatureExtractor& operator= (const FeatureExtractor&) = delete;

//     void extract(const vector<cv::Mat>& samples, vector<cv::Mat>& ft_vec);
//     void extract(const vector<ImageFile>& samples, vector<cv::Mat>& ft_vec);

    template<typename T>
    void extract(const vector<T>& samples, vector<cv::Mat>& ft_vec);

    void setFeatures(const FeatureFlag& flag);

    void setGISTParams(const GISTParams& params);

    // Not used
    void extract(const cv::Mat& sample, vector<vector<float>>& features);

    void mergeFeature(const vector<vector<vector<float>>>& features_src,
                             vector<vector<float>>& features_dst);

private:
    const int GIST_WIDTH  = 256;
    const int GIST_HEIGHT = 256;

    int w = GIST_WIDTH;
    int h = GIST_HEIGHT;

    vector<ImgFeature::Ptr> feature_extractors;
};

template<typename T>
void FeatureExtractor::extract(const vector<T>& samples, vector<cv::Mat>& ft_vec)
{
    int ft_id = 0;
    for (const auto& ex : feature_extractors) {
        cout << "Extracting feature " << ++ft_id << endl;

        int ft_dim = ex->size();
        cv::Mat features(samples.size(), ft_dim, CV_32FC1);

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
_CLANY_END

#endif // IMG_DESCRIPTOR_H
