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
    Gist(int width, int height) : w(width), h(height) {
#ifdef _MSC_VER
        n_orients.assign({8, 8, 8, 8});
#endif
        for (int i = 0; i < n_scale; i++) {
            dim += n_blocks * n_blocks * n_orients[i];
        }
    }
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
    int w = 256, h = 256;
    int n_blocks = 4;
    int n_scale = 4;
#ifdef _MSC_VER
    vector<int> n_orients;
#else
    vector<int> n_orients = {8, 8, 8, 8};
#endif
    int dim = 0;
};
_CLANY_END

#endif // GIST_H
