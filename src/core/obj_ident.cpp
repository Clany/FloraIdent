#include <QDir>
#include <QDirIterator>
#include "core/dist_learning.h"
#include "core/obj_ident.h"

using namespace std;
using namespace cv;
using namespace cls;

void FloraIdent::changeSettings(const array<bool, FEATURES_NUM>& features,
                                const GISTParams& gist_params,
                                bool clear_features)
{
    using FeatureFlag = FeatureExtractor::FeatureFlag;

    int ft_flag = 0;
    if (features[0]) ft_flag |= FeatureFlag::GIST_DESC;
    if (features[1]) ft_flag |= FeatureFlag::LAPL_RGB;
    if (features[2]) ft_flag |= FeatureFlag::HSV_HIST;
    if (features[3]) ft_flag |= FeatureFlag::FOURIER_HIST;
    if (features[4]) ft_flag |= FeatureFlag::HOUGH_HIST;

    ft_extor.setFeatures(static_cast<FeatureFlag>(ft_flag));
    ft_extor.setGISTParams(gist_params);

    if (clear_features) clearFeatures();
}

bool FloraIdent::loadTrainSet(const string& dir, bool has_precompute_fts)
{
    QStringList name_filters;
    name_filters << "*.jpg" << "*.jpeg" << "*.bmp" << "*.tiff" << "*.png";
    QDirIterator dir_iter(dir.c_str(), name_filters,
                          QDir::Files, QDirIterator::Subdirectories);

    QString old_cat("");
    int label = -1;
    int count = 0;
    while (dir_iter.hasNext()) {
        auto file_name = dir_iter.next();
        auto cate_name = dir_iter.fileInfo().dir().dirName();
        if (cate_name != old_cat) {
            ++label;
            old_cat = cate_name;
            train_set.cat_name.push_back(cate_name.toStdString());
            if (count) train_set.cat_count.push_back(count);
            count = 0;
        }

        train_set.images.emplace_back(file_name.toStdString());
        train_set.labels.push_back(label);
        ++count;
    }
    train_set.cat_count.push_back(count);

    // Load precompute features if exist
    if (has_precompute_fts) {
        FileStorage ifs;
        Mat scale_mat;
        QDirIterator dir_iter(dir.c_str(), QDir::Files);
        while (dir_iter.hasNext()) {
            auto file_name = dir_iter.next().toStdString();
            if (file_name.find("train_features") != string::npos) {
                ifs.open(file_name, FileStorage::READ);
                ifs["features"]  >> train_fts;
                ifs["scale_mat"] >> scale_mat;
                /*ifs["dist_mat"]  >> dist_mat;*/
            }
            if (file_name.find("svm") != string::npos) {
//                 ml::SVM svm(file_name);
//                 svm_set.push_back(svm);
                svm.readModel(file_name);
            }
        }
        ft_extor.setScaleFacotrs(scale_mat);
        update_status = STATUS::UPDATE;
    }

    // Set category/image mask to all one
    img_mask.resize(train_set.size(),   1);
    cat_mask.resize(train_set.cat_size(), 1);

    if (train_set.empty()) return false;

    return true;
}

void FloraIdent::setTestImg(const Mat& src)
{
    test_img = src.clone();

    update_status = svm.isReady() ? STATUS::UPDATE : STATUS::INITIAL;
    fill(img_mask, 1);
    fill(cat_mask, 1);
    curr_result = "";
}

void FloraIdent::getCandidates(array<Mat, CANDIDATES_SIZE>& candidates)
{
    auto iter = candidates.begin();
    for (auto i = 0; i < CANDIDATES_SIZE; ++i) {
        candidates[i] = train_set.images[cand_idx[i]];
    }
}

void FloraIdent::genTrainFeatures()
{
    using FeatureFlag = FeatureExtractor::FeatureFlag;

    // Set features to use according to current status
    int ft_flag = 0;
    switch (update_status) {
    case STATUS::INITIAL:
        ft_flag |= FeatureFlag::GIST_DESC;
        break;
    case STATUS::UPDATE:
        ft_flag |= FeatureFlag::GIST_DESC;
        ft_flag |= FeatureFlag::LAPL_RGB;
        ft_flag |= FeatureFlag::HSV_HIST;
        ft_flag |= FeatureFlag::FOURIER_HIST;
        ft_flag |= FeatureFlag::HOUGH_HIST;
    }
    ft_extor.setFeatures(static_cast<FeatureFlag>(ft_flag));

    if (update_status == STATUS::INITIAL && !svm.isReady()) {
        // Extract the set of different features
        vector<Mat> train_ft_vec;
        ft_extor.extract(train_set.images, train_ft_vec);

        // Get GIST feature
        train_fts = train_ft_vec[0];
        svm.train(train_fts, train_set.labels);

        // Save to disk
        FileStorage ofs("train_features.xml", FileStorage::WRITE);
        ofs << "features" << train_fts
            << "scale_mat" << ft_extor.getScaleFactors();

        svm.writeModel("svm_model");
        update_status = STATUS::UPDATE;
    }
}

void FloraIdent::genTestFeatures()
{
    vector<Mat> test_ft_vec;
    ft_extor.extract(test_img, test_ft_vec);
    test_ft = test_ft_vec[0];

    // Initialize candidates
    predict();
}


void FloraIdent::updateCandidates(const UserResponse& user_resp, int lambda)
{
    // Get response of each category
    array<int, CANDIDATES_SIZE/2> cat_resp;
    int possible_result = -1;
    for (auto i = 0; i < CANDIDATES_SIZE/2; ++i) {
        cat_resp[i] = user_resp[2*i] + user_resp[2*i+1];
        int label = train_set.labels[cand_idx[2*i]];
        if (cat_resp[i] ==  2) possible_result = label;
        if (cat_resp[i] == -2) cat_mask[label] = -1;
    }

    if (update_status == STATUS::UPDATE) {
        if (possible_result == -1) {
            predict();
            return;
        }

        auto sub_cat = train_set.subCat(possible_result);
        vector<int> idx_vec(sub_cat.second - sub_cat.first);
        iota(idx_vec, sub_cat.first);
        shuffle(idx_vec, rand_engine);

        copy_n(idx_vec.begin(), CANDIDATES_SIZE, cand_idx.begin());
        curr_result = train_set.cat_name[possible_result];

        update_status = STATUS::FINAL;
        return;
    }

    if (update_status == STATUS::FINAL) {
        // Current result is wrong if 3 out of 6 images are not marked as similar
        if (count(user_resp, 1) > 3) return;

        predict();
        update_status = STATUS::UPDATE;
    }
}

void FloraIdent::predict()
{
    // Sort the labels according to probability predicted by SVM
    vector<double> prob_vec;
    vector<int>    label_idx;
    svm.predict(test_ft, prob_vec, true);
    sortIdx(prob_vec, label_idx, CV_SORT_EVERY_ROW | CV_SORT_DESCENDING);
    remove_if(label_idx, [this](int idx) {
        return cat_mask[idx] != 1;
    });

    for (auto i = 0; i < CANDIDATES_SIZE/2; ++i) {
        // Get most 2 similar images
//         auto sub_cat = train_set.subCat(label_idx[i]);
//         vector<double> dist(sub_cat.second - sub_cat.first);
//         for (auto j = sub_cat.first; j < sub_cat.second; ++j) {
//             dist[j-sub_cat.first] = knn.euclideanDist(test_ft, train_fts.row(j));
//         }
//
//         vector<int> cand_idx;
//         sortIdx(dist, cand_idx, CV_SORT_EVERY_ROW | CV_SORT_ASCENDING);
//
//         cand_vec[i*2]   = train_set.images[cand_idx[0] + sub_cat.first];
//         cand_vec[i*2+1] = train_set.images[cand_idx[1] + sub_cat.first];

        // Use 2 random images
        auto sub_cat = train_set.subCat(label_idx[i]);

        uniform_int_distribution<> uid(sub_cat.first, sub_cat.second-1);
        int first_idx, second_idx;
        do {
            first_idx  = uid(rand_engine);
            second_idx = uid(rand_engine);
        } while (first_idx == second_idx);

        cand_idx[i*2]   = first_idx;
        cand_idx[i*2+1] = second_idx;
    }

    // Save result
    curr_result = train_set.cat_name[label_idx.front()];
}

void FloraIdent::clearData()
{
    train_set.clear();

    if (!train_fts.empty()) train_fts.release();
    if (!dist_mat.empty())  dist_mat.release();

    svm_set.clear();
}

void FloraIdent::clearFeatures()
{
    svm_set.clear();
    train_fts.release();
    dist_mat.release();
}