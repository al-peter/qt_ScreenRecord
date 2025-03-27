#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QLabel>
#include <opencv2/opencv.hpp>
#include <QPixmap>
#include <QDebug>

// Widget that displays the captured video frames in the UI.
class VideoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VideoWidget(QWidget *parent = nullptr);  // Constructor with optional parent widget

public slots:
    void updateFrame(const cv::Mat &frame);  // Slot to update the widget with a new frame

private:
    QLabel *label;  // QLabel to display the captured video frames
    QPixmap currentPixmap;  // Current frame stored as a QPixmap to display on QLabel
};

#endif // VIDEOWIDGET_H
