#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <memory>
#include <functional>
#include <numeric>
#include <opencv2/opencv.hpp>
#include "flora_ident_config.h"
#include "clany/timer.hpp"


_CLANY_BEGIN
const int CANDIDATES_SIZE = 6;

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