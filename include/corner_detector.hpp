#ifndef CORNER_DETECTOR_HPP
#define CORNER_DETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <vector>

class CornerDetector {
public:
    // interactive selection of 4 points (image coords)
    void selectInitialPoints(cv::Mat &image, std::vector<cv::Point2f> &points);

    // detect strong edge (via Laplacian magnitude) in a window around point
    void detectZeroCrossings(const cv::Mat &image, const cv::Point2f &point, cv::Point2f &refined_point);

    // compute homography using user's DLT (wrapper uses computeHomographyDLT)
    void computeHomography(const std::vector<cv::Point2f> &points, cv::Mat &homography);

    // convenience: refine points -> homography
    void processImage(const cv::Mat &image, std::vector<cv::Point2f> &refined_points, cv::Mat &homography);
};

#endif // CORNER_DETECTOR_HPP