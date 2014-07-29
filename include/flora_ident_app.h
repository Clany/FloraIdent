#ifndef FLORA_IDENT_APP_H
#define FLORA_IDENT_APP_H

#include <QImage>
#include "common.h"
#include "core/flora_ident.h"

_CLANY_BEGIN
class FloraIdentApp
{
// private:
//     enum class ProcessState {BEGIN, DATASET, QUERY, GEN_FEATURE};

public:
    bool loadDataset(const string& dir_name);
    void loadQueryImg(const string& file_name);

    void setSelectionRegion(int x, int y, int width, int height);
    void extractFeatures();
    void updateCandidates(const vector<int>& user_resp);

    void getCandidates(array<QImage, CANDIDATES_SIZE>& cands);
    string getResult();

private:
    void matToQImage(const cv::Mat& src, QImage& dst);

    template<typename InputIterator, typename OutputIterator>
    void matToQImage(InputIterator first, InputIterator last, OutputIterator result);

    FloraIdent flora_ident;
    cv::Mat query_img;
    array<cv::Mat, CANDIDATES_SIZE> candidates;
};

template<typename InputIterator, typename OutputIterator>
void FloraIdentApp::matToQImage(InputIterator first, InputIterator last, OutputIterator result)
{
    static_assert(is_input_iterator<InputIterator>::value,   "first/last are not an input iterator");
    static_assert(is_output_iterator<OutputIterator>::value, "result is not an output iterator");
    CHECK_TYPE(decltype(*first),  cv::Mat, "Input type is not Mat");
    CHECK_TYPE(decltype(*result), QImage,  "Output type is not QImage");

    while (first != last) {
        matToQImage(*first, *result);
        ++first;
        ++result;
    }
}
_CLANY_END

#endif // FLORA_IDENT_APP_H