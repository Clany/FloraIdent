#ifndef OBJ_IDENT_APP_H
#define OBJ_IDENT_APP_H

#include <QImage>
#include <QRect>
#include "common.h"
#include "core/obj_ident.h"
#include "clany/traits.hpp"

_CLANY_BEGIN
struct FloraIdentSettings
{
    FloraIdentSettings()
      : gist_params(DEFAULT_GIST_PARAMS), learning_rate(30),
        clear_precomp_features(false) {
        used_features.fill(true);
    }

    array<bool, FEATURES_NUM> used_features;
    GISTParams gist_params;
    int learning_rate;
    bool clear_precomp_features;
    bool display_file_name;
};

class FloraIdentApp {
public:
    FloraIdentSettings& getSettings() { return settings; }
    void setSettings(const FloraIdentSettings& settings);
    void setLearningRate(int learning_rate);

    bool loadDataset(const string& dir_name);
    void loadQueryImg(const string& file_name);

    void setSelectionRegion(int x, int y, int width, int height);
    void setSelectionRegion(const QRect& rect);
    void extractFeatures();
    void updateCandidates(const UserResponse& user_resp);

    void getCandidates(array<QImage, CANDIDATES_SIZE>& cands,
                       array<QString, CANDIDATES_SIZE>& cand_fns);
    string getResult();

private:
    template<typename InputIterator, typename OutputIterator>
    void matToQImage(InputIterator first, InputIterator last, OutputIterator result) {
        static_assert(is_input_iterator<InputIterator>::value, "first/last are not an input iterator");
        static_assert(is_output_iterator<OutputIterator>::value, "result is not an output iterator");
        static_assert(is_same<decltype(*first), cv::Mat&>::value, "Input type is not Mat");
        static_assert(is_same<decltype(*result), QImage&>::value, "Output type is not QImage");

        while (first != last) {
            matToQImage(*first, *result);
            ++first;
            ++result;
        }
    }

    void matToQImage(const cv::Mat& src, QImage& dst);

    FloraIdent         flora_ident;
    FloraIdentSettings settings;

    cv::Mat query_img;
    array<cv::Mat, CANDIDATES_SIZE> candidates;
    array<string, CANDIDATES_SIZE>  cand_fns;
};
_CLANY_END

#endif // OBJ_IDENT_APP_H