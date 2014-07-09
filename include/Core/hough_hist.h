#ifndef HOUGH_HIST_H
#define HOUGH_HIST_H

#include "common.h"

_CLANY_BEGIN
const int R_BINS = 7;   // Distance bins
const int A_BINS = 8;   // Angle bins

class HoughHist : public ImgFeature
{
private:
    void getFeature(const cv::Mat& src, vector<float>& feature) override;

    int getDim() override { return R_BINS * A_BINS; }
};
_CLANY_END

#endif // HOUGH_HIST_H