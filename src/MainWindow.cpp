#include "MainWindow.h"
#include "homography.hpp"
#include <QFileDialog>
#include <QMouseEvent>
#include <QPixmap>
#include <QMessageBox>
#include <algorithm>
#include "MainWindow.h"
#include "homography.hpp"
#include <QFileDialog>
#include <QMouseEvent>
#include <QPixmap>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <algorithm>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(1000, 650);
    setWindowTitle("Image Perspective Tool");

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(15);

    // --- Üst kısım: Görüntüler ---
    QHBoxLayout *imageLayout = new QHBoxLayout();
    imageLayout->setSpacing(20);

    originalLabel = new QLabel("Original Image");
    processedLabel = new QLabel("Processed Image");
    magnifierLabel = new QLabel("Magnifier");

    // Ortala yazı ve biraz padding
    for (QLabel* label : {originalLabel, processedLabel, magnifierLabel}) {
        label->setAlignment(Qt::AlignCenter);
        label->setMinimumSize(280, 350);
        label->setMargin(10);
        label->setStyleSheet(
            "QLabel { "
            "background-color: #fafafa; "
            "border: 2px solid #3498db; "
            "border-radius: 12px; "
            "color: #555555; "
            "font-weight: 600; "
            "font-size: 14pt; "
            "}"
            "QLabel::hover {"
            "border-color: #2980b9;"
            "}"
            );
        label->setContentsMargins(10, 10, 10, 10);
    }

    // Magnifier'ı koyu yap, daha belirgin
    magnifierLabel->setStyleSheet(
        "QLabel { "
        "background-color: #222222; "
        "border: 2px solid #3498db; "
        "border-radius: 12px; "
        "color: #eee; "
        "font-weight: 600; "
        "font-size: 14pt; "
        "}"
        "QLabel::hover {"
        "border-color: #2980b9;"
        "}"
        );

    imageLayout->addWidget(originalLabel, 4);
    imageLayout->addWidget(processedLabel, 4);
    imageLayout->addWidget(magnifierLabel, 2);

    mainLayout->addLayout(imageLayout);

    // --- Orta kısım: Butonlar ---
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(25);

    uploadBtn = new QPushButton("Upload Image");
    convertBtn = new QPushButton("Convert to A4");
    saveBtn = new QPushButton("Save Result");
    clearPointsBtn = new QPushButton("Clear Points");

    // Butonlara ortak stil
    QString btnStyle =
        "QPushButton {"
        "  background-color: #3498db;"
        "  color: white;"
        "  font-weight: bold;"
        "  font-size: 14pt;"
        "  padding: 12px 22px;"
        "  border-radius: 14px;"
        "  border: none;"
        "  min-width: 150px;"
        "  box-shadow: 0 4px 6px rgba(52, 152, 219, 0.3);"
        "  transition: background-color 0.3s ease;"
        "}"
        "QPushButton:hover {"
        "  background-color: #2980b9;"
        "}"
        "QPushButton:disabled {"
        "  background-color: #b0cce8;"
        "  color: #666666;"
        "  box-shadow: none;"
        "}";

    uploadBtn->setStyleSheet(btnStyle);
    convertBtn->setStyleSheet(btnStyle);
    saveBtn->setStyleSheet(btnStyle);
    clearPointsBtn->setStyleSheet(btnStyle);

    btnLayout->addWidget(uploadBtn);
    btnLayout->addWidget(convertBtn);
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(clearPointsBtn);

    mainLayout->addLayout(btnLayout);

    // --- Genel pencere arka planı ---
    central->setStyleSheet("background-color: #f5f8fa;");

    // Buton bağlantıları
    connect(uploadBtn, &QPushButton::clicked, this, &MainWindow::onUploadClicked);
    connect(convertBtn, &QPushButton::clicked, this, &MainWindow::onConvertClicked);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveClicked);
    connect(clearPointsBtn, &QPushButton::clicked, this, &MainWindow::onClearAllPointsClicked);

    // Event filter
    originalLabel->installEventFilter(this);

    // Başlangıçta butonlar kapalı
    convertBtn->setEnabled(false);
    saveBtn->setEnabled(false);
    updateLabelSizes();

}

MainWindow::~MainWindow()
{
}

// MainWindow.cpp içinde:

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateLabelSizes();

    // Eğer görüntü varsa yeniden göster (ölçek doğru kalsın diye)
    if (!originalImage.empty())
        showMatOnLabel(displayImage, originalLabel);

    if (!resultImage.empty())
        showMatOnLabel(resultImage, processedLabel);
}

void MainWindow::updateLabelSizes()
{
    // Pencere genişliği ve yüksekliği al
    int w = this->width() - 30;   // layout margin ve padding çıkarıldı
    int h = this->height() - 150; // butonlar ve üst margin çıkarıldı, ayarlayabilirsin

    // A4 oranı: genişlik / yükseklik
    constexpr double A4_RATIO = 210.0 / 297.0; // ~0.707

    // processedLabel için maksimum boyutları hesapla
    // Örneğin pencere yüksekliğinin %80'i kadar yer verelim
    int maxProcessedHeight = static_cast<int>(h * 0.8);
    int maxProcessedWidth = static_cast<int>(w * 0.3);  // genişliğin %30'u kadar

    // processedLabel boyutlarını A4 oranına göre ayarla
    int processedWidth = maxProcessedWidth;
    int processedHeight = static_cast<int>(processedWidth / A4_RATIO);

    if (processedHeight > maxProcessedHeight) {
        processedHeight = maxProcessedHeight;
        processedWidth = static_cast<int>(processedHeight * A4_RATIO);
    }

    processedLabel->setFixedSize(processedWidth, processedHeight);

    // Kalan genişliği diğer iki label'a böl (original ve magnifier)
    int remainingWidth = w - processedWidth - 40; // aradaki spacing 20*2=40

    int originalWidth = remainingWidth * 0.65;
    int magnifierWidth = remainingWidth - originalWidth;

    // minimum boyutlar
    originalWidth = std::max(originalWidth, 200);
    magnifierWidth = std::max(magnifierWidth, 150);

    int labelHeight = processedHeight; // Hepsi aynı yükseklik olsun

    originalLabel->setFixedSize(originalWidth, labelHeight);
    magnifierLabel->setFixedSize(magnifierWidth, labelHeight);
}


void MainWindow::onClearAllPointsClicked() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
        this,
        "Confirm Clear",
        "Tüm noktaları silmek istediğinize emin misiniz?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        this->resetAllPoints();
        QMessageBox::information(this, "Cleared", "Tüm noktalar silindi.");
        update();
    }
}

QImage MainWindow::matToQImage(const cv::Mat &mat) {
    if (mat.empty()) return QImage();
    if (mat.type() == CV_8UC3) {
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
        return QImage((const unsigned char*)rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
    }
    else if (mat.type() == CV_8UC1) {
        return QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
    }
    return QImage();
}

void MainWindow::showMatOnLabel(const cv::Mat &mat, QLabel *label) {
    QImage img = matToQImage(mat);
    if (img.isNull()) return;

    QSize lblSize = label->size();
    QPixmap pm = QPixmap::fromImage(img.scaled(lblSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    label->setPixmap(pm);

    imageScale = double(pm.width()) / double(mat.cols);
    imgOffsetX = (label->width() - pm.width()) / 2;
    imgOffsetY = (label->height() - pm.height()) / 2;
}

void MainWindow::onUploadClicked() {
    QString path = QFileDialog::getOpenFileName(this, "Open Image", QString(), "Images (*.png *.jpg *.jpeg *.bmp)");
    if (path.isEmpty()) return;
    originalImage = cv::imread(path.toStdString(), cv::IMREAD_COLOR);
    if (originalImage.empty()) return;

    displayImage = originalImage.clone();
    clickedPoints.clear();
    refinedPoints.clear();

    convertBtn->setEnabled(false);
    saveBtn->setEnabled(false);

    showMatOnLabel(displayImage, originalLabel);
    processedLabel->clear();
}

void MainWindow::onConvertClicked() {
    if (clickedPoints.size() != 4) return;

    cv::Mat gray;
    cv::cvtColor(originalImage, gray, cv::COLOR_BGR2GRAY);

    refinedPoints.resize(4);
    for (int i = 0; i < 4; ++i) {
        detector.detectZeroCrossings(gray, clickedPoints[i], refinedPoints[i]);
    }

    cv::Point2f center(0, 0);
    for (const auto& pt : refinedPoints) center += pt;
    center *= (1.0f / refinedPoints.size());

    std::sort(refinedPoints.begin(), refinedPoints.end(), [&center](const cv::Point2f &a, const cv::Point2f &b) {
        double angleA = std::atan2(a.y - center.y, a.x - center.x);
        double angleB = std::atan2(b.y - center.y, b.x - center.x);
        return angleA < angleB;
    });

    std::vector<Eigen::Vector3d> src, dst;
    for (const auto& p : refinedPoints) {
        src.emplace_back(p.x, p.y, 1.0);
    }

    dst = {
        {0, 0, 1},
        {210, 0, 1},
        {210, 297, 1},
        {0, 297, 1}
    };

    Eigen::Matrix3d H = computeHomographyDLT(src, dst);

    int w = 210;
    int h = 297;
    resultImage = cv::Mat(h, w, CV_8UC3, cv::Scalar(255, 255, 255));

    warpPerspectiveCustom(originalImage, resultImage, H);

    showMatOnLabel(resultImage, processedLabel);
    saveBtn->setEnabled(true);
}

void MainWindow::onSaveClicked() {
    if (!resultImage.empty())
        cv::imwrite("roamlab_a4.jpg", resultImage);
}

void MainWindow::resetAllPoints() {
    clickedPoints.clear();
    refinedPoints.clear();

    // Görüntüyü orijinal hale getir (işaretleri kaldır)
    if (!originalImage.empty()) {
        displayImage = originalImage.clone();
        showMatOnLabel(displayImage, originalLabel);
    }
    convertBtn->setEnabled(false);
    saveBtn->setEnabled(false);
}

void MainWindow::mousePressOnImage(int x, int y) {
    int lx = x - imgOffsetX;
    int ly = y - imgOffsetY;
    if (lx < 0 || ly < 0) return;
    int imgx = static_cast<int>(std::round(lx / imageScale));
    int imgy = static_cast<int>(std::round(ly / imageScale));
    imgx = std::clamp(imgx, 0, originalImage.cols - 1);
    imgy = std::clamp(imgy, 0, originalImage.rows - 1);

    if (clickedPoints.size() < 4) {
        clickedPoints.emplace_back(static_cast<float>(imgx), static_cast<float>(imgy));

        cv::Mat temp = originalImage.clone();
        for (const auto &p : clickedPoints) {
            cv::circle(temp, p, 5, cv::Scalar(0, 0, 255), 2, cv::LINE_AA);
        }
        showMatOnLabel(temp, originalLabel);

        if (clickedPoints.size() == 4)
            convertBtn->setEnabled(true);
    }
}

void MainWindow::updateMagnifier(int imgX, int imgY) {
    if (originalImage.empty()) return;

    int zoomWidth = magnifierLabel->width();
    int zoomHeight = magnifierLabel->height();

    int srcWidth = zoomWidth / 4;
    int srcHeight = zoomHeight / 4;

    int x0 = std::clamp(imgX - srcWidth / 2, 0, originalImage.cols - srcWidth);
    int y0 = std::clamp(imgY - srcHeight / 2, 0, originalImage.rows - srcHeight);

    cv::Rect roi(x0, y0, srcWidth, srcHeight);
    cv::Mat zoomPart = originalImage(roi);

    cv::Mat zoomed;
    cv::resize(zoomPart, zoomed, cv::Size(zoomWidth, zoomHeight), 0, 0, cv::INTER_LINEAR);

    QImage qimg(zoomed.data, zoomed.cols, zoomed.rows, zoomed.step, QImage::Format_BGR888);
    magnifierLabel->setPixmap(QPixmap::fromImage(qimg));
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == originalLabel) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *me = static_cast<QMouseEvent*>(event);
            mousePressOnImage(me->pos().x(), me->pos().y());
            return true;
        } else if (event->type() == QEvent::MouseMove) {
            QMouseEvent *me = static_cast<QMouseEvent*>(event);
            int lx = me->pos().x() - imgOffsetX;
            int ly = me->pos().y() - imgOffsetY;
            if (lx >= 0 && ly >= 0) {
                int imgx = std::clamp(int(lx / imageScale), 0, originalImage.cols - 1);
                int imgy = std::clamp(int(ly / imageScale), 0, originalImage.rows - 1);
                updateMagnifier(imgx, imgy);
            }
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}
