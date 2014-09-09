#ifndef IMG_FEATURE_H
#define IMG_FEATURE_H

#include "common.h"

_CLANY_BEGIN
class ImgFeature
{
public:
    using Ptr = shared_ptr < ImgFeature > ;

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

#endif // IMG_FEATURE_H