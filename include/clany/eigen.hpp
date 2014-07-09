#ifndef CLANY_EIGEN_HPP
#define CLANY_EIGEN_HPP

#include <iostream>
#include "Eigen/Dense"
#include "Eigen/StdVector"

#define AUX_STR_EXP(__A)  #__A
#define AUX_STR(__A)      AUX_STR_EXP(__A)

#define AUX_STRW_EXP(__A)  L#__A
#define AUX_STRW(__A)      AUX_STRW_EXP(__A)

#define EIGEN_VERSION AUX_STR(EIGEN_WORLD_VERSION) "." AUX_STR(EIGEN_MAJOR_VERSION) "." AUX_STR(EIGEN_MINOR_VERSION)

namespace Eigen {

template<typename T, int Rows>
using Vector = Matrix<T, Rows, 1, 0, Rows, 1>;

template<typename T, int Rows, int Cols>
using Matrix_ = Matrix<T, Rows, Cols, 0, Rows, Cols>;

typedef Vector<int,    5> Vector5i;
typedef Vector<float,  5> Vector5f;
typedef Vector<double, 5> Vector5d;
typedef Vector<int,    6> Vector6i;
typedef Vector<float,  6> Vector6f;
typedef Vector<double, 6> Vector6d;
typedef Vector<int,    7> Vector7i;
typedef Vector<float,  7> Vector7f;
typedef Vector<double, 7> Vector7d;
typedef Vector<int,    8> Vector8i;
typedef Vector<float,  8> Vector8f;
typedef Vector<double, 8> Vector8d;
typedef Vector<int,    9> Vector9i;
typedef Vector<float,  9> Vector9f;
typedef Vector<double, 9> Vector9d;

typedef Matrix<int,    3, 4> Matrix34i;
typedef Matrix<float,  3, 4> Matrix34f;
typedef Matrix<double, 3, 4> Matrix34d;


//////////////////////////////////////////////////////////////////////////
template<typename T, int M>
inline void CholeskyDecomp(const Matrix_<T, M, M>& A, Matrix_<T, M, M>& K)
{
    LLT<Matrix_<T, M, M>> kkt(A);
    K = kkt.matrixL();
}


template<typename T, int M, int N>
inline void QRDecomp(const Matrix_<T, M, N>& A, Matrix_<T, M, M>& Q, Matrix_<T, N, N>& R)
{
    static_assert(M >= N || M == Dynamic, "QR decomposition need rows >= cols");

    HouseholderQR<Matrix_<T, M, N>> qr(A);
    Q = qr.householderQ();
    R = qr.matrixQR().block(0, 0, N, N).template triangularView<Upper>();

    if (R(0, 0) < 0) {
        Q = -Q;
        R = -R;
    }
}


template<typename T, int M, int N>
inline void RQDecomp(const Matrix_<T, M, N>& A, Matrix_<T, M, M>& R, Matrix_<T, N, N>& Q)
{
    static_assert(M <= N || N == Dynamic, "RQ decomposition need cols >= rows");

    HouseholderQR<Matrix_<T, N, M>> qr(A.colwise().reverse().transpose());
    Q = Reverse<Matrix_<T, N, N>, Vertical>(qr.householderQ().transpose());
    R = Reverse<Matrix_<T, M, M>, BothDirections>(
        qr.matrixQR().block(0, 0, M, M).template triangularView<Upper>().transpose());

    if (R(0, 0) < 0) {
        Q = -Q;
        R = -R;
    }
}


template<typename T, int M, int N, int P>
inline void SVDDecomp(const Matrix_<T, M, N>& A, Vector<T, P>& s,
                      Matrix_<T, M, P>& U, Matrix_<T, N, P>& V)
{
    static_assert((M < N ? M : N) == P, "singular values size is wrong!");

    JacobiSVD<Matrix_<T, M, N>> svd(A, ComputeFullU | ComputeFullV);
    s = svd.singularValues();
    U = svd.matrixU().block(0, 0, M, P);
    V = svd.matrixV().block(0, 0, N, P);
}


// Solve linear system Ax = b using SVD (if b is not provided, use b = 0)
template<typename T, int M, int N>
inline void SVDSolve(const Matrix_<T, M, N>& A, Vector<T, N>& x,
                     const Vector<T, M>& b = Vector<T, M>::Zero())
{
    JacobiSVD<Matrix_<T, M, N>> svd(A, ComputeFullU | ComputeFullV);
    if (b.isZero()) {
        x = svd.matrixV().block(0, N - 1, N, 1);
    } else {
        x = svd.solve(b);
    }
}

}
#endif // CLANY_EIGEN_HPP