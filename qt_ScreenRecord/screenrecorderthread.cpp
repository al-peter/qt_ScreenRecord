#include "screenrecorderthread.h"
#include <QMetaObject>
#include <QProcess>
#include <QDebug>

ScreenRecorderThread::ScreenRecorderThread(QObject *parent)
    : QThread(parent), recording(false), ffmpegProcess(nullptr), windowId(0)
{
}

void ScreenRecorderThread::startRecording(bool isWindowCaptureChecked)
{
    if (recording) return;  // Don't start a new recording if one is already in progress

    recording = true;
    outputFile = "/home/orangepi/Documents/Projects/qt_ScreenRecord/recordings/" + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss") + "_screen.mp4";

    QStringList arguments;

    if (isWindowCaptureChecked)
    {
        // Command for capturing a specific window using X11 grab
        arguments << "-y"
                  << "-f" << "x11grab"
                  << "-video_size" << "640x480"
                  << "-i" << ":0.0+640,327"
                  << "-r" << "30"
                  << "-c:v" << "h264_v4l2m2m"
                  << "-b:v" << "2M"
                  << "-pix_fmt" << "yuv420p"
                  << outputFile;
    } else
    {
        // Command for capturing the entire screen
        arguments << "-y"
                  << "-f" << "x11grab"
                  << "-video_size" << "1920x1080"
                  << "-i" << ":0.0"
                  << "-r" << "30"
                  << "-c:v" << "h264_v4l2m2m"
                  << "-b:v" << "2M"
                  << "-pix_fmt" << "yuv420p"
                  << outputFile;
    }

    // Start the ffmpeg process
    ffmpegProcess = new QProcess(this);
    ffmpegProcess->start("ffmpeg", arguments);

    // Check if the process started successfully
    if (!ffmpegProcess->waitForStarted())
    {
        qWarning() << "Failed to start the recording process.";
        emit errorOccurred("Failed to start the recording process.");
    } else
    {
        qInfo() << "Recording started: " << outputFile;
        emit recordingStarted(outputFile);  // Emit signal to indicate recording started
    }
}

void ScreenRecorderThread::runFFmpeg()
{
    qputenv("DISPLAY", ":1");

    ffmpegProcess = new QProcess(this);
    QStringList arguments;

    // Set up ffmpeg arguments based on whether full-screen or window capture is selected
    if (fullScreen)
    {
        arguments << "-y"
                  << "-f" << "x11grab"
                  << "-video_size" << "1920x1080"
                  << "-i" << ":0.0"
                  << "-r" << "30"
                  << "-c:v" << "h264_v4l2m2m"
                  << "-b:v" << "2M"
                  << "-pix_fmt" << "yuv420p"
                  << outputFile;
    } else
    {
        arguments << "-y"
                  << "-f" << "x11grab"
                  << "-video_size" << "640x480"
                  << "-i" << QString(":%0").arg(windowId)  // Specify window capture ID
                  << "-r" << "30"
                  << "-c:v" << "h264_v4l2m2m"
                  << "-b:v" << "2M"
                  << "-pix_fmt" << "yuv420p"
                  << outputFile;
    }

    ffmpegProcess->setProcessChannelMode(QProcess::MergedChannels);

    // Handle the output from ffmpeg process
    connect(ffmpegProcess, &QProcess::readyReadStandardOutput, [this]()
    {
        qDebug() << "FFmpeg Output: " << ffmpegProcess->readAllStandardOutput();
    });

    // Handle error output from ffmpeg process
    connect(ffmpegProcess, &QProcess::readyReadStandardError, [this]()
    {
        qDebug() << "FFmpeg Error: " << ffmpegProcess->readAllStandardError();
    });

    // Start the ffmpeg process with arguments
    ffmpegProcess->start("ffmpeg", arguments);

    if (!ffmpegProcess->waitForStarted())
    {
        emit errorOccurred("FFmpeg process failed to start.");
        return;
    }

    qDebug() << "FFmpeg process started!";

    // Wait for the ffmpeg process to finish
    if (!ffmpegProcess->waitForFinished())
    {
        emit errorOccurred("FFmpeg process failed to finish.");
    }

    int exitCode = ffmpegProcess->exitCode();
    qDebug() << "FFmpeg process finished with exit code: " << exitCode;

    emit recordingStopped();  // Emit signal to indicate recording stopped
}

void ScreenRecorderThread::stopRecording()
{
    if (!recording) return;  // Don't stop if recording isn't in progress

    if (ffmpegProcess) {
        ffmpegProcess->terminate();
        // If process doesn't terminate in time, forcefully kill it
        if (!ffmpegProcess->waitForFinished(3000))
        {
            qWarning() << "FFmpeg process did not finish in time. Forcing kill.";
            ffmpegProcess->kill();
        }

        recording = false;
        qInfo() << "Recording stopped.";
        emit recordingStopped();  // Emit signal to indicate recording stopped
    }
}

void ScreenRecorderThread::setWindowId(uint64_t winId)
{
    this->windowId = winId;  // Set the window ID for window capture
}
