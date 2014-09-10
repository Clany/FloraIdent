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

class FeatureExtractor {
public:
    enum FeatureFlag { GIST_DESC = 1, LAPL_RGB = 2, HSV_HIST = 4,
                       FOURIER_HIST = 8, HOUGH_HIST = 16 };

    FeatureExtractor();

    void extract(const vector<cv::Mat>& samples, vector<cv::Mat>& ft_vec);

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

_CLANY_END

#endif // IMG_DESCRIPTOR_H
