#include <random>
#include <cmath>
#include <QDir>
#include "clany/eigen.hpp"
#include "Eigen/Sparse"
#include <opencv2/core/eigen.hpp>
#include "core/flora_ident.h"
#include "clany/timer.hpp"

using namespace std;
using namespace cv;
using namespace clany;
using namespace Eigen;

const float EPSF = numeric_limits<float>::epsilon();
const double EPSD = numeric_limits<double>::epsilon();
const float MAX_FLOAT = numeric_limits<float>::max();
const double MAX_DOUBLE = numeric_limits<double>::max();


void FloraIdent::loadTrainSet(const string& dir, bool has_precompute_fts)
{
    QDir curr_dir(dir.c_str());
    auto loc_list = curr_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    int label = 0;
    for (const auto& loc : loc_list) {
        QDir cat_dir(curr_dir.filePath(loc));
        auto cat_list = cat_dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for (const auto& cat : cat_list) {
            QDir file_dir(cat_dir.filePath(cat));
            auto file_list = file_dir.entryList(QDir::Files);
            for (const auto& fn : file_list) {
                train_set.data.push_back(imread(file_dir.filePath(fn).toStdString()));
                train_set.labels.push_back(label);
            }
            cat_set.push_back(cat.toStdString());
            ++label;
        }
    }
    // Load precompute features if exist
    if (has_precompute_fts) {
        curr_dir.cdUp();
        auto file_list = curr_dir.entryList(QDir::Files);
        for (const auto& fn : file_list) {
            auto file = curr_dir.filePath(fn).toStdString();
            if (file.find("train_features.xml") != string::npos) {
                FileStorage ifs(file, FileStorage::READ);
                read(ifs["features"], train_fts);
                read(ifs["dist_mat"], dist_mat);
            }
            if (file.find("svm_set") != string::npos) {
                ml::SVM svm(file);
                svm_set.push_back(svm);
            }
        }
    }
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
        FeatureExtractor::extract(train_set.data, train_ft_vec);

        size_t train_sz = train_set.size();
        size_t cat_sz = cat_set.size();
        train_fts.create(train_sz, cat_sz*train_ft_vec.size(), CV_64FC1);
        for (int i = 0; i < train_ft_vec.size(); ++i) {
            train_ft_vec[i].convertTo(train_ft_vec[i], CV_64F);

            ml::SVM svm;
            svm.train(train_ft_vec[i], train_set.labels);
            for (int j = 0; j < train_sz; ++j) {
                ml::SVMNode node;
                node.dim = train_ft_vec[i].cols;
                node.values = train_ft_vec[i].ptr<double>(j);
                svm.predict(node, train_fts.ptr<double>(j) +i*cat_sz, true);
            }
            svm_set.push_back(svm);
        }

        // Initialize distance matrix
        initDistMat();

        // Save features and distance matrix on disk
        FileStorage ofs("train_features.xml", FileStorage::WRITE);
        write(ofs, "features", train_fts);
        write(ofs, "dist_mat", dist_mat);

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
    FeatureExtractor::extract({test_img}, test_ft_vec);

    size_t cat_sz = cat_set.size();
    test_ft.create(1, cat_sz*test_ft_vec.size(), CV_64FC1);
    for (int i = 0; i < svm_set.size(); ++i) {
        test_ft_vec[i].convertTo(test_ft_vec[i], CV_64F);

        ml::SVMNode node;
        node.dim = test_ft_vec[i].cols;
        node.values = test_ft_vec[i].ptr<double>();
        svm_set[i].predict(node, test_ft.ptr<double>() + i*cat_sz, true);
    }

    // Initialize candidates
    predict();
}


void FloraIdent::updateCandidates(const vector<int>& user_resp)
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

    // Random shuffle all the pairs and update distance matrix
    // until converge, i.e., 3 full passes are made without improvements
    int min_err_count = numeric_limits<int>::max();
    int iter_count = 0;
    vector<int> cands(CANDIDATES_SIZE);
    iota(cands.begin(), cands.end(), 0);
    while (true) {
        int err_count = 0;
        shuffle(cands.begin(), cands.end(), default_random_engine(RAND_SEED));
        for (int i = 0; i < CANDIDATES_SIZE; ++i) {
            int idx = cands[i];
            if (user_resp[idx]) {
                err_count += updateDistMat(test_ft, 1, train_fts.row(cand_idx[idx]), user_resp[idx], 50);
            }
        }

        shuffle(pairs.begin(), pairs.end(), default_random_engine(RAND_SEED));
        for (const auto& pair : pairs) {
            err_count += updateDistMat(train_fts.row(cand_idx[pair.first]), user_resp[pair.first],
                                       train_fts.row(cand_idx[pair.second]), user_resp[pair.second]);
        }
        cout << err_count << endl;
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
    int idx = cvRound(classifier.predict(test_ft, 10, neighbors, [&A](InputArray _a1, InputArray _a2) {
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

// Implementation of a distance learning algorithm presented in
// "Regularized Distance Metric Learning: Theory and Algorithm"
// by Rong Jin, Shijun Wang, and Yang Zhou
void FloraIdent::initDistMat()
{
    dist_mat.create(train_fts.cols, train_fts.cols, CV_64FC1);
    dist_mat.setTo(0);
    Mat A = dist_mat.clone();

    const int train_sz = train_fts.rows;
    const int pair_sz = train_fts.rows * (train_fts.rows - 1) / 2;
    vector<pair<int, int>> pairs;
    pairs.reserve(pair_sz);
    for (int i = 0; i < train_sz; ++i) {
        for (int j = i + 1; j < train_sz; ++j) {
            pairs.emplace_back(i, j);
        }
    }

    // Random shuffle all the pairs and update distance matrix
    // until converge, i.e., 3 full passes are made without improvements
    int min_err_count = numeric_limits<int>::max();
    int iter_count = 0;
    while (true) {
        shuffle(pairs.begin(), pairs.end(), default_random_engine(RAND_SEED));
        int err_count = 0;
        CPUTimer timer;
        for (const auto& pair : pairs) {
            err_count += updateDistMat(train_fts.row(pair.first), train_set.labels[pair.first],
                                       train_fts.row(pair.second), train_set.labels[pair.second], 0.1);
        }
        timer.elapsed("Elapsed time");
        cout << err_count << endl;
        if (err_count < min_err_count) {
            min_err_count = err_count;
            iter_count = 0;
            A = dist_mat.clone();
        }
        ++iter_count;
        if (iter_count > 3) {
            break;
        }
    }
    dist_mat = A.clone();

    Mat eigen_vals;
    eigen(dist_mat, eigen_vals);
    while (any_of(eigen_vals.begin<double>(), eigen_vals.end<double>(), [](double val) {
        return val < 0;
    })) {
        nearestSPD(dist_mat, dist_mat);
        eigen(dist_mat, eigen_vals);
    }
}

int FloraIdent::updateDistMat(const Mat& x1, int y1, const Mat& x2, int y2, double lambda)
{
    Mat A = dist_mat;
    int yt = y1 == y2 ? 1 : -1;
    Mat delta_x = x1 - x2;
#ifndef NDEBUG
    double dist = (delta_x * A).dot(delta_x);
#endif
    if (yt * (1 - (delta_x * A).dot(delta_x)) > 0) return 0;

    double lambda_t = lambda;
    if (yt > 0) {
        MatrixXd eigen_A;
        VectorXd eigen_x, eigen_b;
        cv2eigen(A, eigen_A);
        cv2eigen(delta_x.t(), eigen_x);

        ConjugateGradient<MatrixXd> cg;
        cg.setMaxIterations(1000);
        cg.setTolerance(1e-6);

        cg.compute(eigen_A);
        eigen_b = cg.solve(eigen_x);
        double u = 1.0 / eigen_x.dot(eigen_b);

        lambda_t = min(lambda, u);
    }
    A -= lambda_t * yt * delta_x.t() * delta_x;

    return 1;
}

void FloraIdent::nearestSPD(const Mat& src, Mat& dst)
{
    Mat B = (src + src.t()) / 2.0;

    Mat w, u, vt, S;
    SVD::compute(B, w, u, vt);
    S = Mat::diag(w);

    Mat H = vt.t() * S * vt;

    dst = (B + H) / 2.0;
    dst = (dst + dst.t()) / 2.0;

    // Test positive definite
    MatrixXd A, L;
    cv2eigen(dst, A);

    int i = 1;
    while (!CholeskyDecomp(A, L)) {
        double min_eig = A.eigenvalues().real().minCoeff();
        A += (eps(min_eig) - min_eig * i * i) * MatrixXd::Identity(A.rows(), A.cols());
        ++i;
    }
    eigen2cv(A, dst);
}

void FloraIdent::clearData()
{
    cat_set.clear();
    train_set.data.clear();
    train_set.labels.clear();

    if (!train_fts.empty()) train_fts.setTo(0);
    if (!dist_mat.empty())  dist_mat.setTo(0);

    svm_set.clear();
}