#include <QDir>
#include <QDirIterator>
#include "clany/algorithm.hpp"
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
    while (dir_iter.hasNext()) {
        auto file_name = dir_iter.next();
        auto cate_name = dir_iter.fileInfo().dir().dirName();
        if (cate_name != old_cat) {
            ++label;
            old_cat = cate_name;
            cat_set.push_back(cate_name.toStdString());
        }

        train_set.data.emplace_back(file_name.toStdString());
        train_set.labels.push_back(label);
    }

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
                ifs["dist_mat"]  >> dist_mat;
                ifs["scale_mat"] >> scale_mat;
            }
            if (file_name.find("svm_set") != string::npos) {
                ml::SVM svm(file_name);
                svm_set.push_back(svm);
            }
        }
        ft_extor.setScaleFacotrs(scale_mat);
    }

    if (train_set.empty()) return false;

    return true;
}

void FloraIdent::setTestImg(const Mat& src)
{
    test_img = src.clone();
}

void FloraIdent::getCandidates(array<Mat, CANDIDATES_SIZE>& candidates)
{
    auto iter = candidates.begin();
    for (const auto& idx : cand_idx) {
        *iter = train_set.data[idx];
        ++iter;
    }
}

void FloraIdent::genTrainFeatures()
{
    if (svm_set.empty()) {
        vector<Mat> train_ft_vec;
        ft_extor.extract(train_set.data, train_ft_vec);

        size_t train_sz = train_set.size();
        size_t cat_sz = cat_set.size();
        train_fts.create(train_sz, cat_sz*train_ft_vec.size(), CV_64FC1);
        for (auto i = 0u; i < train_ft_vec.size(); ++i) {
            train_ft_vec[i].convertTo(train_ft_vec[i], CV_64F);

            ml::SVM svm;
            cout << "Training SVM for feature " << i+1 << "..." << endl;
            svm.train(train_ft_vec[i], train_set.labels);
            cout << string(50, '-') << endl;
            for (auto j = 0u; j < train_sz; ++j) {
                ml::SVMNode node;
                node.dim = train_ft_vec[i].cols;
                node.values = train_ft_vec[i].ptr<double>(j);
                svm.predict(node, train_fts.ptr<double>(j) + i*cat_sz, true);
            }
            svm_set.push_back(svm);
        }

        // Initialize distance matrix
        initDistMat(dist_mat, train_fts, train_set.labels);

        // Save features and distance matrix on disk
        FileStorage ofs("train_features.xml", FileStorage::WRITE);
        ofs << "features"  << train_fts << "dist_mat" << dist_mat
            << "scale_mat" << ft_extor.getScaleFactors();

        int i = 0;
        for (const auto& svm : svm_set) {
            svm.writeModel(string("svm_set_") + to_string(i++));
        }
    }

    // Train the knn classifier based on transformed features
    Mat label_mat(train_set.size(), 1, CV_32SC1, train_set.labels.data());
    classifier.train(train_fts, label_mat);
}

void FloraIdent::genTestFeatures()
{
    vector<Mat> test_ft_vec;
    ft_extor.extract(test_img, test_ft_vec);

    size_t cat_sz = cat_set.size();
    test_ft.create(1, cat_sz*test_ft_vec.size(), CV_64FC1);
    for (auto i = 0u; i < svm_set.size(); ++i) {
        test_ft_vec[i].convertTo(test_ft_vec[i], CV_64F);

        ml::SVMNode node;
        node.dim = test_ft_vec[i].cols;
        node.values = test_ft_vec[i].ptr<double>();
        svm_set[i].predict(node, test_ft.ptr<double>() + i*cat_sz, true);
    }

    // Initialize candidates
    predict();
}


void FloraIdent::updateCandidates(const UserResponse& user_resp, int lambda)
{
    Mat A = dist_mat.clone();

    vector<pair<int, int>> pairs;
    for (int i = 0; i < CANDIDATES_SIZE; ++i) {
        for (int j = i + 1; j < CANDIDATES_SIZE; ++j) {
            if (user_resp[i] && user_resp[j]) {
                pairs.emplace_back(i, j);
            }
        }
    }

    // Random shuffle all the pairs and update distance matrix until converge
    int min_err_count = INT32_MAX;
    int iter_count = 0;
    vector<int> cands(CANDIDATES_SIZE);
    iota(cands.begin(), cands.end(), 0);
    for (;;) {
        int err_count = 0;
        // Update according to test-cand pairs
        shuffle(cands, rand_engine);
        for (int i = 0; i < CANDIDATES_SIZE; ++i) {
            int idx = cands[i];
            if (user_resp[idx]) {
                err_count += updateDistMat(dist_mat, test_ft, 1,
                                           train_fts.row(cand_idx[idx]), user_resp[idx],
                                           lambda);
            }
        }

        // Update based on cand-cand pairs
        shuffle(pairs, rand_engine);
        for (const auto& pair : pairs) {
            err_count += updateDistMat(dist_mat,
                                       train_fts.row(cand_idx[pair.first]),
                                       user_resp[pair.first],
                                       train_fts.row(cand_idx[pair.second]),
                                       user_resp[pair.second]);
        }
#ifndef NDEBUG
        cout << err_count << endl;
#endif
        if (err_count < min_err_count) {
            min_err_count = err_count;
            iter_count = 0;
            A = dist_mat.clone();
        }
        ++iter_count;
        if (iter_count > CANDIDATES_SIZE) {
            break;
        }
    }
    dist_mat = A.clone();

    predict();
}

string FloraIdent::predict()
{
//     Mat ft_mat;
//     test_ft.convertTo(ft_mat, CV_32F);

    vector<int> neighbors;
    Mat A = dist_mat;
    int idx = cvRound(classifier.predict(test_ft, 10, neighbors,
        [&A](InputArray _a1, InputArray _a2) {
        Mat a1 = _a1.getMat();
        Mat a2 = _a2.getMat();

        CV_Assert((a1.size() == a2.size()) &&
                  (a1.rows == 1 || a1.cols == 1) &&
                  (a2.rows == 1 || a2.cols == 1));

        auto delta = a1 - a2;
        return (delta*A).dot(delta);
    }));

    neighbors.resize(CANDIDATES_SIZE);
    cand_idx = move(neighbors);

    return cat_set[idx];
}

void FloraIdent::clearData()
{
    cat_set.clear();
    train_set.data.clear();
    train_set.labels.clear();

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