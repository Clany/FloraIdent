#ifndef FOURIER_HIST_H
#define FOURIER_HIST_H

#include "common.h"
#include "img_feature.h"

_CLANY_BEGIN
constexpr int FOURD_BINS = 8;
constexpr int FOURW_BINS = 32;

class FourierHist : public ImgFeature
{
private:
    void getFeature(const cv::Mat& src, vector<float>& feature) override;

    int getDim() override { return FOURD_BINS + FOURW_BINS; }

    void fourierTransform(const cv::Mat& src, cv::Mat& dst);
};
_CLANY_END

#endif // FOURIER_HIST_H
