#ifndef FLORA_IDENT_APP_H
#define FLORA_IDENT_APP_H

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>
#include "clany/clany_macros.h"

_CLANY_BEGIN
class FloraIdentApp
{
public:
    void loadDataset(const string& file_name);
    void loadQueryImg(const string& file_name);

    const cv::Mat& getQueryImg() { return query_img; };

    void setSelectionRegion(int x, int y, int width, int height);

private:
    vector<cv::Mat> dataset;
    cv::Mat query_img;
    cv::Mat cropped_img;
};
_CLANY_END

#endif // FLORA_IDENT_APP_H