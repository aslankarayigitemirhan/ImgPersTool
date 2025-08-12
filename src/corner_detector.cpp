#include "corner_detector.hpp"
#include "homography.hpp"
#include <cmath>
#include <atomic>

#include "util.hpp"


static std::vector<cv::Point2f> temp_points;
static std::atomic<int> point_count(0);


// Laplacian implementation (returns float image)
static void applyLaplacian(const cv::Mat &input, cv::Mat &output) {
    CV_Assert(input.type() == CV_8UC1);
    output.create(input.size(), CV_32FC1);
    cv::Mat padding_image;
    copyMakeBorder(input, padding_image, 1, 1, 1, 1, cv::BORDER_REPLICATE);
    float filterdata[3][3] = {{-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1}};
    cv::Mat padding_float;
    padding_image.convertTo(padding_float, CV_32FC1);
    for (int row = 0; row < input.rows; row++) {
        for (int column = 0; column < input.cols; column++) {
            float sumInsideLaplacianKernel = 0.0f;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    sumInsideLaplacianKernel += padding_float.at<float>(row + i, column + j) * filterdata[i][j];
                }
            }
            output.at<float>(row, column) = sumInsideLaplacianKernel;
        }
    }
}

void CornerDetector::detectZeroCrossings(const cv::Mat &image, const cv::Point2f &point, cv::Point2f &refined_point) {
    cv::Mat laplacian;
    applyLaplacian(image, laplacian);
    int x = static_cast<int>(std::round(point.x));
    int y = static_cast<int>(std::round(point.y));
    int half_window = 15;
    int x_start = std::max(0, x - half_window);
    int x_end = std::min(laplacian.cols-1, x + half_window);
    int y_start = std::max(0, y - half_window);
    int y_end = std::min(laplacian.rows-1, y + half_window);

    float max_gradient = 0.0f;
    cv::Point2f closest_point = point;

    for (int i = y_start; i <= y_end; ++i) {
        for (int j = x_start; j <= x_end; ++j) {
            float gradient = std::fabs(laplacian.at<float>(i, j));
            if (gradient > max_gradient) {
                max_gradient = gradient;
                closest_point = cv::Point2f(static_cast<float>(j), static_cast<float>(i));
            }
        }
    }

    refined_point = closest_point;
}
