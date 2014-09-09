#ifndef HOUGH_HIST_H
#define HOUGH_HIST_H

#include "common.h"
#include "img_feature.h"

_CLANY_BEGIN
constexpr int R_BINS = 7;   // Distance bins
constexpr int A_BINS = 8;   // Angle bins

class HoughHist : public ImgFeature
{
private:
    void getFeature(const cv::Mat& src, vector<float>& feature) override;

    int getDim() override { return R_BINS * A_BINS; }
};
_CLANY_END

#endif // HOUGH_HIST_H