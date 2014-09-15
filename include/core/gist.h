#ifndef GIST_H
#define GIST_H

#include "common.h"
#include "img_feature.h"
#include "libgist.h"

_CLANY_BEGIN
struct GISTParams {
    bool        use_color;
    int         width;
    int         height;
    int         blocks;
    int         scale;
    vector<int> orients;
    int         size;
};

const GISTParams DEFAULT_GIST_PARAMS {false, 256, 256, 4, 4, {8, 8, 8, 8}, 512};

class GIST : public ImgFeature {
public:
    GIST() = default;

    explicit GIST(const GISTParams& gist_params) {
        setParams(gist_params);
    }

    static void setParams(const GISTParams& gist_params);

private:
    void getFeature(const cv::Mat& src, vector<float>& result) override;
    int  getDim() override { return params.size; };

    static GISTParams params;
};
_CLANY_END

#endif // GIST_H
