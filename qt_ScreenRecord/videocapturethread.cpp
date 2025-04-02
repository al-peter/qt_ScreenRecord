#include "videocapturethread.h"
#include <QDebug>
#include <QElapsedTimer>
#include <QCoreApplication>

Q_DECLARE_METATYPE(cv::Mat);

VideoCaptureThread::VideoCaptureThread(QObject *parent)
    : QThread(parent),
      running(false),
      testPool("/home/orangepi/Documents/Projects/qt_ScreenRecord/qt_ScreenRecord/model/RK3588/yolov5n.rknn", threadNum),
      lastFpsUpdateTime(0)
{
    // Initialize testPool and check for errors
    if (testPool.init() != 0)
    {
        printf("rknnPool init fail!\n");
    }

    // Initialize GStreamer
    gst_init(nullptr, nullptr);

    // Open the camera using V4L2 API
    cap.open("/dev/video0", cv::CAP_V4L2);
    if (!cap.isOpened())
    {
        qWarning("Failed to open camera!");
    }

    // Start timer for FPS calculation
    frameTimer.start();

    // Set the camera's properties (FPS, width, height)
    cap.set(cv::CAP_PROP_FPS, 30);  // Set max FPS
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);  // Set frame width
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);  // Set frame height

    // Define the GStreamer pipeline (appsrc -> H.265 -> RTP -> UDP)
    std::string pipeline =
        "appsrc ! videoconvert ! video/x-raw,format=NV12 ! "
        "mpph265enc ! h265parse ! rtph265pay ! "
        "udpsink host=127.0.0.1 port=5601";

    // Open the GStreamer pipeline for writing the video stream
    writer.open(pipeline, cv::CAP_GSTREAMER, 0, 15, cv::Size(640, 480), true);
    if (!writer.isOpened())
    {
        qWarning("Error: Failed to open GStreamer pipeline!");
    }
}

VideoCaptureThread::~VideoCaptureThread()
{
    stop();  // Stop the thread
    wait();  // Wait for the thread to finish
}

void VideoCaptureThread::stop()
{
    running = false;  // Stop the thread by setting the flag
    qInfo() << "Video capture stopped.";
}

void VideoCaptureThread::run()
{
    running = true;
    cv::Mat frame;

    QElapsedTimer timer;
    timer.start();

    const int frameDelay = 33;  // Delay between frames to maintain 30 FPS

    while (running)
    {
        // Capture a frame from the camera
        if (cap.read(frame) == false)
            break;  // Break if the frame could not be captured

        // Add frame to test pool for processing
        if (testPool.put(frame) != 0)
        {
            qWarning() << "Error: testPool.put() failed!";
            break;
        }

        //qDebug() << "Retrieving processed frame from testPool...";
        if (frameCount >= threadNum && testPool.get(frame) != 0)
        {
            qWarning() << "Error: testPool.get() failed!";
            break;
        }
        //qDebug() << "Frame successfully processed by testPool!";

        frameCount++;  // Increment frame count

        // Calculate FPS based on elapsed time
        qint64 elapsed = frameTimer.restart();
        double fps = 1000.0 / elapsed;  // FPS calculation

        // Update FPS every 100ms
        qint64 currentTime = timer.elapsed();
        if (currentTime - lastFpsUpdateTime > 100)
        {
            emit fpsUpdated(static_cast<int>(fps));  // Emit FPS update
            lastFpsUpdateTime = currentTime;  // Update last FPS update time
        }

        //qDebug() << "Before writer.write(), frame size: " << frame.cols << "x" << frame.rows;
        if (!frame.empty())
        {
            writer.write(frame);
        }
        else
        {
            qWarning() << "Received empty frame!";
        }
        //qDebug() << "After writer.write(), frame size: " << frame.cols << "x" << frame.rows;

        // Emit the captured frame for further processing
        emit frameCaptured(frame);

        // Delay to ensure the frame processing doesn't exceed the camera's FPS
        qint64 frameTime = frameTimer.elapsed();
        if (frameTime < frameDelay)
        {
            msleep(frameDelay - frameTime);  // Sleep to maintain desired FPS
        }

        //QCoreApplication::processEvents();  // Process any pending events
    }
}

