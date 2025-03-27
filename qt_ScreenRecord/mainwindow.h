#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include "videocapturethread.h"
#include "videowidget.h"
#include "screenrecorderthread.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);  // Constructor with optional parent widget
    ~MainWindow();  // Destructor

protected:
    void closeEvent(QCloseEvent *event) override;  // Handles window close event

private:
    VideoCaptureThread *captureThread;  // Thread for capturing video frames
    ScreenRecorderThread *screenRecorder;  // Thread for screen recording
    VideoWidget *videoWidget;  // Widget for displaying video frames
    QLabel *fpsLabel;  // Label for displaying FPS (Frames Per Second)
    QPushButton *recordWindowButton;  // Button to toggle window recording
    QPushButton *recordScreenButton;  // Button to toggle full screen recording

    uint64_t videoWidgetId;  // Unique identifier for the video widget

private slots:
    void updateFPS(int fps);  // Slot to update FPS label
    void toggleWindowRecording(bool checked);  // Slot to start/stop window recording
    void toggleScreenRecording(bool checked);  // Slot to start/stop screen recording
};

#endif // MAINWINDOW_H
