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
    outputFile = "../recordings/" + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss") + "_screen.mp4";

    QStringList arguments;

    if (isWindowCaptureChecked)
    {
        // Command for capturing a specific window using X11 grab
        arguments << "-y"
                  << "-f" << "x11grab"
                  << "-video_size" << "640x480"
                  << "-i" << ":0.0+640,327"
                  << "-r" << "30"
                  << "-c:v" << "libx264"
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
                  << "-c:v" << "libx264"
                  << "-b:v" << "2M"
                  << "-pix_fmt" << "yuv420p"
                  << outputFile;
    }

    //qputenv("DISPLAY", ":1");

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

void ScreenRecorderThread::stopRecording()
{
    if (!recording) return;  // Don't stop if recording isn't in progress

    if (ffmpegProcess) {

        qInfo() << "Stopping recording...";
        ffmpegProcess->write("q"); // q to stop
        ffmpegProcess->closeWriteChannel();

        if (!ffmpegProcess->waitForFinished(5000))
        {
            qWarning() << "FFmpeg did not close in time. Attempting terminate()...";
            ffmpegProcess->terminate();

            if (!ffmpegProcess->waitForFinished(5000))
            {
                qWarning() << "FFmpeg still not closing. Forcing kill()...";
                ffmpegProcess->kill();
            }
        }

        int exitCode = ffmpegProcess->exitCode();
        qInfo() << "FFmpeg exited with code: " << exitCode;

        recording = false;
        emit recordingStopped();  // Emit signal to indicate recording stopped
    }
}

void ScreenRecorderThread::setWindowId(uint64_t winId)
{
    this->windowId = winId;  // Set the window ID for window capture
}
