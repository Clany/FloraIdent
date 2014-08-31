#ifndef COMMON_H
#define COMMON_H

#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <memory>
#include <functional>
#include <numeric>
#include <chrono>
#include <opencv2/opencv.hpp>
#include "flora_ident_config.h"
#include "clany/traits.hpp"

#ifdef _MSC_VER
#  define constexpr const
#endif


_CLANY_BEGIN
constexpr int CANDIDATES_SIZE = 6;
const     uint RAND_SEED = chrono::system_clock::now().time_since_epoch().count();

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