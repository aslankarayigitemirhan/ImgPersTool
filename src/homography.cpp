#include "homography.hpp"
#include "util.hpp"
#include <Eigen/SVD>
#include <cmath>
#include <cassert>
using namespace std;
using namespace cv;
using namespace Eigen;
Vector3d toEigen(const Point2f& pt) {
    return Vector3d(pt.x, pt.y, 1.0);
}

// Normalize edilmiş DLT homography hesaplama
Matrix3d computeHomographyDLT(const vector<Vector3d>& src, const vector<Vector3d>& dst) {
    assert(src.size() == dst.size() && src.size() >= 4);

    MatrixXd A(2 * src.size(), 9);

    for (size_t i = 0; i < src.size(); ++i) {
        const auto& s = src[i];
        const auto& d = dst[i];

        double x = s(0), y = s(1), w = s(2);
        double xp = d(0), yp = d(1), wp = d(2);


        A(2*i,   0) = 0;     A(2*i,   1) = 0;     A(2*i,   2) = 0;
        A(2*i,   3) = -wp*x; A(2*i,   4) = -wp*y; A(2*i,   5) = -wp*w;
        A(2*i,   6) = yp*x;  A(2*i,   7) = yp*y;  A(2*i,   8) = yp*w;

        A(2*i+1, 0) = wp*x;  A(2*i+1, 1) = wp*y;  A(2*i+1, 2) = wp*w;
        A(2*i+1, 3) = 0;     A(2*i+1, 4) = 0;     A(2*i+1, 5) = 0;
        A(2*i+1, 6) = -xp*x; A(2*i+1, 7) = -xp*y; A(2*i+1, 8) = -xp*w;
    }

    JacobiSVD<MatrixXd> svd(A, ComputeFullV);
    VectorXd h = svd.matrixV().col(8);
    Matrix3d H;
    H << h(0), h(1), h(2),
        h(3), h(4), h(5),
        h(6), h(7), h(8);
    return H;
}
void warpPerspectiveCustom(const cv::Mat &src, cv::Mat &dst, const Eigen::Matrix3d &H){
        cv::Mat result(3, 3, CV_64F);
        for (int r = 0; r < 3; ++r) {
            double* row_ptr = result.ptr<double>(r);
            for (int c = 0; c < 3; ++c) {
                row_ptr[c] = H(r, c);
            }
        }
        cv::warpPerspective(src,dst,result,cv::Size(210,297),cv::INTER_LINEAR);
}
