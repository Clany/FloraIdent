#ifndef FLORA_IDENT_APP_H
#define FLORA_IDENT_APP_H

#include "common.h"
#include "core/flora_ident.h"

_CLANY_BEGIN
class FloraIdentApp
{
// private:
//     enum class ProcessState {BEGIN, DATASET, QUERY, GEN_FEATURE};

public:
    void loadDataset(const string& dir_name);
    void loadQueryImg(const string& file_name);

    void setSelectionRegion(int x, int y, int width, int height);
    void extractFeatures();
    void updateCandidates(const vector<int> user_resp);

    const array<cv::Mat, CANDIDATES_SIZE>& getCandidates() { return candidates; }
    string getResult();

private:
    FloraIdent flora_ident;
    cv::Mat query_img;
    array<cv::Mat, CANDIDATES_SIZE> candidates;

    bool has_train_fts = false;
};
_CLANY_END

#endif // FLORA_IDENT_APP_H