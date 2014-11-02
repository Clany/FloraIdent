#ifndef DIST_LEARNING_H
#define DIST_LEARNING_H

#include "common.h"

_CLS_BEGIN
// Implementation of a distance learning algorithm presented in
// "Regularized Distance Metric Learning: Theory and Algorithm"
// by Rong Jin, Shijun Wang, and Yang Zhou

void initDistMat(cv::Mat& dist_mat,
                const cv::Mat& train_fts,
                const vector<int>& labels);

int updateDistMat(cv::Mat& dist_mat,
                  const cv::Mat& x1, int y1,
                  const cv::Mat& x2, int y2,
                  double lambda = 1);

void nearestSPD(const cv::Mat& src, cv::Mat& dst);

template<typename T>
inline enable_if_t<is_floating_point<T>::value, T>
eps(T val)
{
    return nextafter(val, numeric_limits<T>::max()) - val;
};
_CLS_END

#endif // DIST_LEARNING_H
