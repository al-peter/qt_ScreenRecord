#ifndef VIDEOCAPTURETHREAD_H
#define VIDEOCAPTURETHREAD_H

#include <QThread>
#include <opencv2/opencv.hpp>
#include <QtNetwork/QUdpSocket>

#include "rkYolov5s.hpp"
#include "rknnPool.hpp"

// Thread responsible for capturing video from the camera, calculating FPS, and sending frames over UDP.
class VideoCaptureThread : public QThread
{
    Q_OBJECT
public:
    explicit VideoCaptureThread(QObject *parent = nullptr);  // Constructor with optional parent widget
    ~VideoCaptureThread();  // Destructor to clean up resources

    void stop();  // Stops the video capture thread

protected:
    void run() override;  // Override the run method to define thread behavior

signals:
    void frameCaptured(const cv::Mat &frame);  // Signal emitted when a frame is captured
    void fpsUpdated(int fps);  // Signal emitted when the FPS is updated

private:
    cv::VideoCapture cap;  // OpenCV VideoCapture object to capture frames from the camera
    bool running;  // Flag indicating whether the thread is running
    int threadNum = 3;  // Number of threads used for processing (default value)

    int frameCount;  // Counter for the number of frames captured
    QElapsedTimer frameTimer;  // Timer to calculate FPS by measuring the time between frame captures
    rknnPool<rkYolov5s, cv::Mat, cv::Mat> testPool;  // Pool to manage the inference task using the RKNN model (YoloV5)

    qint64 lastFpsUpdateTime;  // Time of the last FPS update
};

#endif // VIDEOCAPTURETHREAD_H
