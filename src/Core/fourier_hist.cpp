#include "fourier_hist.h"

using namespace std;
using namespace cv;
using namespace clany;


void FourierHist::getFeature(const Mat& src, vector<float>& hist)
{
    Mat img;
    cvtColor(src, img, CV_BGR2GRAY);
    hist.resize(FOURD_BINS + FOURW_BINS);

    Mat dft_map;
    fourierTransform(img, dft_map);

    // FOURD signature
    array<double, FOURD_BINS> radius;
    radius.back() = min(img.cols, img.rows);
    radius[0] = radius.back() / sqrt(FOURD_BINS);
    for (size_t i = 1; i < FOURD_BINS - 1; ++i) {
        radius[i] = sqrt(i + 1) * radius[0];
    }

    float sum = 0;
    Vec2i center(dft_map.cols/2, dft_map.rows/2);
    for(int y = 0; y < dft_map.rows; ++y)
    {
        float* data = dft_map.ptr<float>(y);
        for(int x = 0; x < dft_map.cols; ++x)
        {
            double dist = norm(Vec2i(x, y), center);
            for (int i = 0; i < FOURD_BINS; ++i) {
                if (dist < radius[i]) {
                    sum += data[x];
                    hist[i] += data[x];
                    break;
                }
            }
        }
    }

    // Normalize sum to 0.5
    for_each(hist.begin(), hist.begin() + FOURD_BINS, [sum](float& value) { value /= 2 * sum; });

    // FOURW signature
    double ang_div = 360.0 / FOURW_BINS;
    for(int y = 0; y < dft_map.rows; ++y)
    {
        float* data = dft_map.ptr<float>(y);
        for(int x = 0; x < dft_map.cols; ++x)
        {
            double dist = norm(Vec2i(x, y), center);
            if (dist < radius.back()) {
                double delta_x = x - center[0];
                double delta_y = y - center[1];

                double angle = atan2(delta_y, delta_x) * 180 / CV_PI;
                if (angle < 0) angle += 360;

                int idx = cvFloor(angle / ang_div);
                hist[idx + FOURD_BINS] += data[x];
            }
        }
    }

    // Normalize sum to 0.5
    for_each(hist.begin() + FOURD_BINS, hist.end(), [sum](float& value) { value /= 2 * sum; });
}


void FourierHist::fourierTransform(const Mat& src, Mat& dst)
{
    Mat padded;                            //expand input image to optimal size
    int m = getOptimalDFTSize(src.rows);
    int n = getOptimalDFTSize(src.cols); // on the border add zero values
    copyMakeBorder(src, padded, 0, m - src.rows, 0, n - src.cols, BORDER_CONSTANT, Scalar::all(0));

    Mat planes[] {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
    Mat dft_map;
    merge(planes, 2, dft_map);

    dft(dft_map, dft_map);
    split(dft_map, planes);
    magnitude(planes[0], planes[1], dft_map);

    dft_map += Scalar::all(1);
    log(dft_map, dft_map);

    dft_map = dft_map(Rect(0, 0, dft_map.cols & -2, dft_map.rows & -2));

    int cx = dft_map.cols/2;
    int cy = dft_map.rows/2;

    // Create a ROI per quadrant
    Mat q0(dft_map, Rect(0, 0, cx, cy));   // Top-Left
    Mat q1(dft_map, Rect(cx, 0, cx, cy));  // Top-Right
    Mat q2(dft_map, Rect(0, cy, cx, cy));  // Bottom-Left
    Mat q3(dft_map, Rect(cx, cy, cx, cy)); // Bottom-Right

    Mat tmp;
    q0.copyTo(tmp);     // swap quadrants (Top-Left with Bottom-Right)
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);     // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);

    normalize(dft_map, dft_map, 0, 1, NORM_MINMAX);
    dst = dft_map;
}