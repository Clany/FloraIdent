#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <array>
#include <vector>
#include <map>
#include <numeric>
#include <algorithm>
#include <functional>
#include <opencv2/opencv.hpp>
#include "cv/svm.h"
#include "clany/timer.hpp"
#include "clany/factory.hpp"


_CLANY_BEGIN
class ImgFeature
{
public:
    using Ptr = shared_ptr<ImgFeature>;

    void extract(const cv::Mat& src, vector<float>& feature)
    {
        getFeature(src, feature);
    };

    int size()
    {
        return getDim();
    };

private:
    virtual void getFeature(const cv::Mat& src, vector<float>& feature) = 0;

    virtual int getDim() = 0;
};
_CLANY_END
#endif // COMMON_H
