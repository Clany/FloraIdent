#include "core/color_hist.h"

using namespace std;
using namespace placeholders;
using namespace cv;
using namespace clany;


void LaplRGB::getFeature(const Mat& src, vector<float>& hist)
{
    hist.resize(RGB_BINS * RGB_BINS * RGB_BINS);

    Mat wmap;
    Laplacian(src, wmap, CV_32F);

    int div = cvCeil(256.f / RGB_BINS);

    int nr = src.rows;
    int nc = src.cols * src.channels();
    if (src.isContinuous()) nc = nr * nc, nr = 1;

    for (int y = 0; y < nr; ++y) {
    	const uchar* data = src.ptr<uchar>(y);
        float* w_ptr = wmap.ptr<float>(y);
        for (int x = 0; x < nc; x += 3) {
            int B = data[x] / div;
            int G = data[x + 1] / div;
            int R = data[x + 2] / div;
            int idx = B + G * RGB_BINS + R * RGB_BINS * RGB_BINS;
            float w = pow(w_ptr[x], 2) + pow(w_ptr[x + 1], 2) + pow(w_ptr[x + 2], 2);

            hist[idx] += w;
    	}
    }

    // Normalize sum to 1
    float hist_sum = accumulate(hist.begin(), hist.end(), 0.f);
    transform(hist.begin(), hist.end(), hist.begin(), bind(divides<float>(), _1, hist_sum));
}


void ProbRGB::getFeature(const Mat& src, vector<float>& hist)
{
    hist.resize(RGB_BINS*RGB_BINS*RGB_BINS);

    Mat reduced;
    reduceRGB(src, reduced);

    Mat wmap;
    getWMap(reduced, wmap);

    int div = cvCeil(256.f / RGB_BINS);

    int nr = src.rows;
    int nc = src.cols * src.channels();
    if (src.isContinuous()) nc = nr * nc, nr = 1;

    for (int y = 0; y < nr; ++y) {
        const uchar* data = src.ptr<uchar>(y);
        float* w_ptr = wmap.ptr<float>(y);
        for (int x = 0; x < nc; x += 3) {
            int B = data[x] / div;
            int G = data[x + 1] / div;
            int R = data[x + 2] / div;
            int idx = B + G * RGB_BINS + R * RGB_BINS * RGB_BINS;
            float w = pow(w_ptr[x], 2) + pow(w_ptr[x + 1], 2) + pow(w_ptr[x + 2], 2);
            hist[idx] += w;
        }
    }

    // Normalize sum to 1
    float hist_sum = accumulate(hist.begin(), hist.end(), 0.f);
    transform(hist.begin(), hist.end(), hist.begin(), bind(divides<float>(), _1, hist_sum));
}


void ProbRGB::reduceRGB(const cv::Mat& src, cv::Mat& dst)
{
    dst = src.clone();

    int div = cvCeil(256.f / RGB_BINS);

    int nr = src.rows;
    int nc = src.cols * src.channels();
    if (src.isContinuous()) nc = nr * nc, nr = 1;

    for (int y = 0; y < nr; ++y) {
        const uchar* src_ptr = src.ptr<uchar>(y);
        uchar* dst_ptr = dst.ptr<uchar>(y);
        for (int x = 0; x < nc; ++x) {
            dst_ptr[x] = src_ptr[x] / div * div + div / 2;
        }
    }
}


void ProbRGB::getWMap(const Mat& src, Mat& wmap)
{
    wmap.create(src.size(), CV_32FC3);

    // Get window size
    int wsize = cvRound(min(src.cols, src.rows) / 100.0);
    wsize = wsize / 2 * 2 + 1;
    if (wsize < 5) wsize = 5;
    if (wsize > 9) wsize = 9;

    Mat padded;
    copyMakeBorder(src, padded, wsize / 2, wsize / 2, wsize / 2, wsize / 2, BORDER_REFLECT101);

    Mat mask;
    for (int y = 0; y < src.rows; ++y) {
        const Vec3b* data = src.ptr<Vec3b>(y);
        Vec3f* w_ptr = wmap.ptr<Vec3f>(y);
        for (int x = 0; x < src.cols; ++x) {
            compare(padded(Rect(x, y, wsize, wsize)),
                    Mat_<Vec3b>(wsize, wsize, data[x]),
                    mask, CMP_EQ);
            Scalar_<float> w = sum(mask / 255);
            w_ptr[x][0] = w[0];
            w_ptr[x][1] = w[1];
            w_ptr[x][2] = w[2];
    	}
    }

    divide(1.0, wmap, wmap);
}


void HSVHist::getFeature(const Mat& src, vector<float>& hist)
{
    hist.resize(H_BINS * S_BINS * V_BINS);

    Mat hsv;
    cvtColor(src, hsv, CV_BGR2HSV_FULL);

    int div_H = 256 / H_BINS;
    int div_S = 256 / S_BINS;
    int div_V = 256 / V_BINS;

    int nr = src.rows;
    int nc = src.cols * src.channels();
    if (src.isContinuous()) nc = nr * nc, nr = 1;

    for (int y = 0; y < nr; ++y) {
    	uchar* data = hsv.ptr<uchar>(y);
        for (int x = 0; x < nc; x += 3) {
            int H = data[x] / div_H;
            int S = data[x + 1] / div_S;
            int V = data[x + 2] / div_V;
            int idx = H + S * H_BINS + V * H_BINS * S_BINS;
            ++hist[idx];
    	}
    }

    // Normalize sum to 1
    transform(hist.begin(), hist.end(), hist.begin(), bind(divides<float>(), _1, hsv.rows * hsv.cols * 1.f));
}