#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <opencv2/opencv.hpp>
#include "corner_detector.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onUploadClicked();
    void onConvertClicked();
    void onSaveClicked();
    void onClearAllPointsClicked();

private:
    void updateLabelSizes();
    // Ana widgetlar
    QLabel *originalLabel;
    QLabel *processedLabel;
    QLabel *magnifierLabel;

    QLabel *thumb1;
    QLabel *thumb2;
    QLabel *thumb3;

    QPushButton *uploadBtn;
    QPushButton *convertBtn;
    QPushButton *saveBtn;
    QPushButton *clearPointsBtn;

    cv::Mat originalImage;
    cv::Mat displayImage;
    cv::Mat resultImage;

    std::vector<cv::Point2f> clickedPoints;
    std::vector<cv::Point2f> refinedPoints;

    CornerDetector detector;

    // Yardımcılar
    QImage matToQImage(const cv::Mat &mat);
    void showMatOnLabel(const cv::Mat &mat, QLabel *label);
    void resetAllPoints();
    void mousePressOnImage(int x, int y);
    void updateMagnifier(int imgX, int imgY);

    void updateThumbnails();

    double imageScale = 1.0;
    int imgOffsetX = 0;
    int imgOffsetY = 0;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

};

#endif // MAINWINDOW_H
