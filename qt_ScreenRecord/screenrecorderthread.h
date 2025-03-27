#ifndef SCREENRECORDERTHREAD_H
#define SCREENRECORDERTHREAD_H

#include <QThread>
#include <QProcess>
#include <QDebug>
#include <QDateTime>

class ScreenRecorderThread : public QThread
{
    Q_OBJECT

public:
    explicit ScreenRecorderThread(QObject *parent = nullptr);  // Constructor with optional parent widget

    void startRecording(bool isWindowCaptureChecked);  // Start recording
    void stopRecording();  // Stop recording
    void setWindowId(uint64_t winId);  // Set window ID for capturing

signals:
    void recordingStarted(const QString &outputFile);  // Signal emitted when recording starts
    void recordingStopped();  // Signal emitted when recording stops
    void errorOccurred(const QString &error);  // Signal emitted when an error occurs

protected:
    void runFFmpeg();  // Run the ffmpeg process

private:
    bool recording;  // Flag to track if recording is in progress
    QProcess *ffmpegProcess;  // Process to run ffmpeg
    QString outputFile;  // Path to the output file where video is saved
    uint64_t windowId;  // Window ID for window capture
    bool fullScreen = true;  // Flag to determine if the screen or a specific window should be captured
};

#endif // SCREENRECORDERTHREAD_H
