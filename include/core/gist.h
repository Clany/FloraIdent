#ifndef GIST_H
#define GIST_H

#include <array>
#include <vector>
#include <opencv2/opencv.hpp>
#include "common.h"
#include "libgist.h"

_CLANY_BEGIN
class Gist : public ImgFeature {
public:
    Gist() = default;
    Gist(int width, int height) : w(width), h(height) {}
    Gist(const cv::Mat& src) : img(src), w(src.cols), h(src.rows) {}

    void extract(vector<float>& result, int n_blocks, int n_scale, vector<int> n_orientations) const {
        extract(img, result, n_blocks, n_scale, n_orientations.data());
    }

    void extract(vector<float>& result, int n_blocks, int n_scale,
                 const int* n_orientations) const {
        extract(img, result, n_blocks, n_scale, n_orientations);
    }

    static
    void extract(const cv::Mat& src, vector<float>& result,
                 int n_blocks, int n_scale, const int* n_orientations);

private:    
    void getFeature(const cv::Mat& src, vector<float>& feature) override;    
    int getDim() override;

    cv::Mat img;
    int w, h;
    int n_blocks = 4;
    int n_scale = 3;
    vector<int> n_orients = {8, 8, 4};
};
_CLANY_END

#endif // GIST_H
