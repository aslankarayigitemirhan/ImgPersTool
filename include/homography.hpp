#ifndef HOMOGRAPHY_HPP
#define HOMOGRAPHY_HPP

#include <Eigen/Dense>
#include <opencv2/opencv.hpp>
#include <vector>

// Compute homography using DLT. src and dst are vectors of homogeneous Eigen::Vector3d.
Eigen::Matrix3d computeHomographyDLT(const std::vector<Eigen::Vector3d>& src, const std::vector<Eigen::Vector3d>& dst);

// Custom warp using inverse mapping and bilinear interpolation. H maps source -> destination.
void warpPerspectiveCustom(const cv::Mat &src, cv::Mat &dst, const Eigen::Matrix3d &H);

#endif // HOMOGRAPHY_HPP