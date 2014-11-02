#include "clany/algorithm.hpp"
#include "clany/timer.hpp"
#include "clany/eigen.hpp"
#include "Eigen/Sparse"
#include <opencv2/core/eigen.hpp>
#include "core/dist_learning.h"

using namespace std;
using namespace cv;
using namespace Eigen;
using namespace cls;

void cls::initDistMat(Mat& dist_mat, const Mat& train_fts, const vector<int>& labels)
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

    // Random shuffle all the pairs and update distance matrix until
    // error count is less than 5%
    int min_err_count = INT32_MAX;
    while (true) {
        shuffle(pairs, rand_engine);
        int err_count = 0;
        CPUTimer timer;
        for (const auto& pair : pairs) {
            err_count += updateDistMat(dist_mat,
                                       train_fts.row(pair.first),
                                       labels[pair.first],
                                       train_fts.row(pair.second),
                                       labels[pair.second], 0.1);
        }
        timer.delta("Elapsed time");
        float ratio = static_cast<float>(err_count) / pair_sz;

        cout.setf(ios::left);
        cout << "Error count: " << setw(9) << err_count
             << ratio * 100 << "%" << endl;
        if (err_count < min_err_count) {
            min_err_count = err_count;
            A = dist_mat.clone();
        }

        if (ratio < 0.05) break;
    }
    dist_mat = A.clone();

    Mat eigen_vals;
    eigen(dist_mat, eigen_vals);
    while (any_of(eigen_vals.begin<double>(), eigen_vals.end<double>(),
        [](double val) {
        return val < 0;
    })) {
        nearestSPD(dist_mat, dist_mat);
        eigen(dist_mat, eigen_vals);
    }
}

int cls::updateDistMat(Mat& dist_mat, const Mat& x1, int y1,
                       const Mat& x2, int y2, double lambda)
{
    Mat& A = dist_mat;
    int yt = y1 == y2 ? 1 : -1;
    Mat delta_x = x1 - x2;

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

void cls::nearestSPD(const cv::Mat& src, cv::Mat& dst)
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
        A += (eps(min_eig) - min_eig * i * i) *
            MatrixXd::Identity(A.rows(), A.cols());
        ++i;
    }
    eigen2cv(A, dst);
}