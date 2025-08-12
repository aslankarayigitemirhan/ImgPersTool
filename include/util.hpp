#ifndef UTIL_HPP
#define UTIL_HPP

#include <opencv2/opencv.hpp>
#include <Eigen/Dense>

// Convert Eigen 3x3 matrix to cv::Mat (double)
inline cv::Mat eigenToCv(const Eigen::Matrix3d &M) {
    cv::Mat out(3,3,CV_64F);
    for(int r=0;r<3;++r) for(int c=0;c<3;++c) out.at<double>(r,c)=M(r,c);
    return out;
}

// Convert cv::Mat (3x3 double) to Eigen
inline Eigen::Matrix3d cvToEigen(const cv::Mat &M) {
    Eigen::Matrix3d E = Eigen::Matrix3d::Identity();
    for(int r=0;r<3;++r) for(int c=0;c<3;++c) E(r,c)=M.at<double>(r,c);
    return E;
}

#endif // UTIL_HPP