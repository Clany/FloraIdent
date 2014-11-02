#include "core/hough_hist.h"

using namespace std;
using namespace cv;
using namespace cls;


void HoughHist::getFeature(const Mat& src, vector<float>& hist)
{
    Mat img;
    cvtColor(src, img, CV_BGR2GRAY);
    hist.resize(R_BINS * A_BINS);

    // Get gradient
    Mat dx_map, dy_map;
    Sobel(img, dx_map, CV_64F, 1, 0);
    Sobel(img, dy_map, CV_64F, 0, 1);

    /*double r_div = norm(Vec2i(img.rows, img.cols)) / R_BINS;*/
    double ang_div = 360.0 / A_BINS;
    double min_r = numeric_limits<double>::max();
    double max_r = numeric_limits<double>::min();
    array<vector<double>, 8> tmp;
    for (int y = 0; y < img.rows; ++y) {
        double* dx = dx_map.ptr<double>(y);
        double* dy = dy_map.ptr<double>(y);
        for (int x = 0; x < img.cols; ++x) {
            Vec2d u(x, y);
            Vec2d dxdy(dx[x], dy[x]);

            Vec2d t(dxdy[1], -dxdy[0]);
            t = normalize(t);

            double r = u.dot(t);
            double angle = atan2(dxdy[1], dxdy[0]) * 180 / CV_PI;
            if (angle < 0) angle += 360;

            if (r < min_r) min_r = r;
            if (r > max_r) max_r = r;
            int idx = cvFloor(angle / ang_div)/* * R_BINS + cvFloor(r / r_div)*/;
            tmp[idx].push_back(r);
        }
    }

    double r_div = (max_r - min_r) / R_BINS;
    const float weight = 1.f / img.rows / img.cols;
    for (int i = 0; i < A_BINS; ++i) {
        const auto& r_vec = tmp[i];
        for (const auto& r : r_vec) {
            int idx = i * R_BINS + cvFloor((r - min_r) / r_div);
            hist[idx] += weight;
        }
    }
}