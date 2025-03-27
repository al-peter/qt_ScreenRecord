#include "mainwindow.h"
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    // Set up the window size
    resize(640, 480);

    // Set up the central widget and layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    setCentralWidget(centralWidget);

    // Create and add the video widget to the layout
    videoWidget = new VideoWidget(this);
    mainLayout->addWidget(videoWidget);

    // Create and set up the FPS label
    fpsLabel = new QLabel("FPS: 0", this);
    fpsLabel->setStyleSheet("color: white; background: black; padding: 5px;");
    fpsLabel->setFixedSize(80, 30);

    // Create buttons for window and screen recording
    recordWindowButton = new QPushButton("Record Window", this);
    recordWindowButton->setCheckable(true);
    recordScreenButton = new QPushButton("Record Screen", this);
    recordScreenButton->setCheckable(true);

    // Layout for buttons and FPS label
    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addWidget(recordWindowButton);
    buttonsLayout->addWidget(recordScreenButton);
    buttonsLayout->addWidget(fpsLabel);
    mainLayout->addLayout(buttonsLayout);

    // Create threads for video capture and screen recording
    captureThread = new VideoCaptureThread(this);
    screenRecorder = new ScreenRecorderThread(this);

    // Connect signals and slots
    connect(captureThread, &VideoCaptureThread::frameCaptured, videoWidget, &VideoWidget::updateFrame);
    connect(captureThread, &VideoCaptureThread::fpsUpdated, this, &MainWindow::updateFPS);
    connect(recordWindowButton, &QPushButton::toggled, this, &MainWindow::toggleWindowRecording);
    connect(recordScreenButton, &QPushButton::toggled, this, &MainWindow::toggleScreenRecording);

    // Start the capture thread
    captureThread->start();
    qDebug() << "Capture thread started successfully.";
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow is being destroyed.";
    // Stop the capture thread if it is running
    if (captureThread && captureThread->isRunning())
    {
        captureThread->stop();
        captureThread->wait();
    }
    // Stop the screen recording thread if it is running
    if (screenRecorder && screenRecorder->isRunning())
    {
        screenRecorder->stopRecording();
        screenRecorder->wait();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Handle the window close event
    if (captureThread && captureThread->isRunning())
    {
        qInfo() << "Stopping capture thread...";
        captureThread->stop();
        captureThread->wait();
    }
    if (screenRecorder && screenRecorder->isRunning())
    {
        qInfo() << "Stopping screen recording...";
        screenRecorder->stopRecording();
        screenRecorder->wait();
    }
    event->accept();
}

void MainWindow::updateFPS(int fps)
{
    // Update the FPS label with the new value
    fpsLabel->setText(QString("FPS: %1").arg(fps));
}

void MainWindow::toggleWindowRecording(bool checked)
{
    // Start/stop recording the window based on the button's state
    if (checked)
    {
        screenRecorder->startRecording(true);  // true - record window
    } else
    {
        screenRecorder->stopRecording();
    }
}

void MainWindow::toggleScreenRecording(bool checked)
{
    // Start/stop recording the whole screen based on the button's state
    if (checked)
    {
        screenRecorder->startRecording(false);  // false - record full screen
    } else
    {
        screenRecorder->stopRecording();
    }
}
