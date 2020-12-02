/*
 * The opencv handler is a wrapper class allowing easy integration into various
 * opencv functionality. It will be implemented later in the project
 */
#ifndef OPENCV_HANDLER_H
#define OPENCV_HANDLER_H


#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QImage>
#include "mainwindow.h"
#include <string>

using std::string;
using namespace cv;

const int iLowH = 0, iHighH = 179, iLowS = 0, iHighS = 255, iLowV = 0, iHighV = 255;

class OpenCV_Handler
{
public:
    OpenCV_Handler();
    void benDemo();
    void playVideo(String filename, QImage *qi);/*
    QImage Mat2QImage(const cv::Mat3b &src);*/
    QImage Mat2QImage(const cv::Mat &mat, QImage::Format format);

};

#endif // OPENCV_HANDLER_H
