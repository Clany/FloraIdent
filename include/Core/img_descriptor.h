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
    static void extract(const cv::Mat& sample, vector<vector<float>>& features);

    static void extract(const vector<cv::Mat>& samples, vector<cv::Mat>& ft_vec);

    static void mergeFeature(const vector<vector<vector<float>>>& features_src,
                             vector<vector<float>>& features_dst);
                             
    static setSize(int width, int height) {
        w = width;
        h = height;
    }

private:
    FeatureExtractor(int width, int height);

    static FeatureExtractor& instance() {
        static FeatureExtractor img_ft(w, h);
        return img_ft;
    }

    vector<ImgFeature::Ptr> feature_extractors;

    static int w;
    static int h;
};

_CLANY_END

#endif // IMG_DESCRIPTOR_H
