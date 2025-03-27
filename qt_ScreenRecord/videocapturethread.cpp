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
    if (testPool.init() != 0)
    {
        printf("rknnPool init fail!\n");
    }

    cap.open("/dev/video0", cv::CAP_V4L2);  // Open camera using V4L2 API
    if (!cap.isOpened())
    {
        qWarning("Failed to open camera!");
    }

    frameTimer.start();  // Start timer for FPS calculation

    cap.set(cv::CAP_PROP_FPS, 30);  // Set max FPS
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);  // Set frame width
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);  // Set frame height
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
    QUdpSocket udpSocket;
    QHostAddress serverAddress("192.168.0.103");  // Set server address
    quint16 serverPort = 27036;  // Set server port

    QElapsedTimer timer;
    timer.start();
    const int MAX_UDP_PACKET_SIZE = 1400;  // Maximum UDP packet size
    int packetId = 0;  // Packet ID for sequential packet sending

    const int frameDelay = 33;  // Delay between frames to maintain 30 FPS

    while (running)
    {

        // Capture a frame from the camera
        if (cap.read(frame) == false)
            break;  // Break if the frame could not be captured

        // Add frame to test pool for processing
        if (testPool.put(frame) != 0)
            break;
        if (frameCount >= threadNum && testPool.get(frame) != 0)
            break;

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

        // Compress frame to JPEG format for UDP transmission
        std::vector<uchar> buffer;
        bool success = cv::imencode(".jpg", frame, buffer, {cv::IMWRITE_JPEG_QUALITY, 50});
        if (!success)
        {
            qWarning("Failed to encode frame to JPEG!");
            continue;
        }

        // Send the frame data via UDP in chunks
        int totalSize = buffer.size();
        int offset = 0;

        while (offset < totalSize)
        {
            // Determine the chunk size to send
            int chunkSize = std::min(MAX_UDP_PACKET_SIZE, totalSize - offset);
            QByteArray chunk(reinterpret_cast<char*>(buffer.data() + offset), chunkSize);

            // Prepare packet header with packet ID
            QByteArray header = QString::number(packetId++).toUtf8();
            QByteArray dataToSend = header + chunk;

            // Send the packet via UDP
            qint64 bytesSent = udpSocket.writeDatagram(dataToSend, serverAddress, serverPort);
            if (bytesSent == -1)
            {
                qWarning() << "Failed to send packet over UDP!";
            }

            offset += chunkSize;  // Update the offset for the next chunk
        }

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
