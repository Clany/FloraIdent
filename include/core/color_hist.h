#ifndef COLOR_HIST_H
#define COLOR_HIST_H

#include "common.h"
#include "img_feature.h"


_CLANY_BEGIN
constexpr int RGB_BINS = 6;
constexpr int H_BINS = 16;
constexpr int S_BINS = 2;
constexpr int V_BINS = 4;

class LaplRGB : public ImgFeature
{
private:
    void getFeature(const cv::Mat& src, vector<float>& feature) override;

    int getDim() override { return RGB_BINS * RGB_BINS * RGB_BINS; }
};


class ProbRGB : public ImgFeature
{
private:
    void getFeature(const cv::Mat& src, vector<float>& feature) override;

    int getDim() override { return RGB_BINS * RGB_BINS * RGB_BINS; }

    void reduceRGB(const cv::Mat& src, cv::Mat& dst);
    void getWMap(const cv::Mat& src, cv::Mat& wmap);
};


class HSVHist : public ImgFeature
{
private:
    void getFeature(const cv::Mat& src, vector<float>& feature) override;

    int getDim() override { return H_BINS * S_BINS * V_BINS; }
};
_CLANY_END

#endif // COLOR_HIST_H
