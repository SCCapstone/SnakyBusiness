/*
 * The opencv handler is a wrapper class allowing easy integration into various
 * opencv functionality. It will be implemented later in the project
 *
#include "opencv_handler.h"

OpenCV_Handler::OpenCV_Handler()
{

}

void OpenCV_Handler::benDemo() {
    VideoCapture cap(0);
            String name = "Control";
            namedWindow(name,WINDOW_NORMAL);

            int iLowH = 0;
            int iHighH = 179;

            int iLowS = 0;
            int iHighS = 255;

            int iLowV = 0;
            int iHighV = 255;

            createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
            createTrackbar("HighH", "Control", &iHighH, 179);

            createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
            createTrackbar("HighS", "Control", &iHighS, 255);

            createTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
            createTrackbar("HighV", "Control", &iHighV, 255);

            while (true) {
                Mat imgOriginal;
                bool bSuccess = cap.read(imgOriginal);
                if (bSuccess == false) {
                    break;
                }
                Mat imgHSV;
                cvtColor(imgOriginal,imgHSV,COLOR_BGR2HSV);

                Mat imgThresholded;
                inRange(imgHSV,Scalar(iLowH,iLowS,iLowV), Scalar(iHighH,iHighS,iHighV), imgThresholded);

                erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
                dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

                dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
                erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

                imshow("Thresholded Image", imgThresholded);
                imshow("Original", imgOriginal);

                if (waitKey(30) == 27) {
                    break;
                }
            }
}
*/
