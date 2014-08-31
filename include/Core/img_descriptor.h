#ifndef IMG_DESCRIPTOR_H
#define IMG_DESCRIPTOR_H

#include "common.h"
#include "gist.h"
#include "color_hist.h"
#include "fourier_hist.h"
#include "hough_hist.h"


_CLANY_BEGIN
class FeatureExtractor {
public:
    FeatureExtractor();

    void extract(const cv::Mat& sample, vector<vector<float>>& features);

    void extract(const vector<cv::Mat>& samples, vector<cv::Mat>& ft_vec);

    void mergeFeature(const vector<vector<vector<float>>>& features_src,
                             vector<vector<float>>& features_dst);

    void setSize(int width, int height) {
        w = width;
        h = height;
    }

private:
    const int GIST_WIDTH  = 256;
    const int GIST_HEIGHT = 256;

    int w = GIST_WIDTH;
    int h = GIST_HEIGHT;

    vector<ImgFeature::Ptr> feature_extractors;
};

_CLANY_END

#endif // IMG_DESCRIPTOR_H
