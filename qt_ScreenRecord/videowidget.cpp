#include "videowidget.h"
#include <QImage>
#include <QVBoxLayout>

VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent)
{
    // Initialize QLabel for displaying video frames
    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // label->setMinimumSize(640, 480);  // Optional minimum size
    label->setScaledContents(true);

    // Create and set up the layout for the widget
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->setMargin(0);  // No margin around the layout
    setLayout(layout);
}

void VideoWidget::updateFrame(const cv::Mat &frame)
{
    // Skip if the frame is empty
    if (frame.empty()) return;

    // Convert the frame from BGR (OpenCV) to RGB (Qt)
    cv::Mat rgbFrame;
    cv::cvtColor(frame, rgbFrame, cv::COLOR_BGR2RGB);
    QImage img(rgbFrame.data, rgbFrame.cols, rgbFrame.rows, rgbFrame.step, QImage::Format_RGB888);

    // Create a QPixmap from the QImage
    currentPixmap = QPixmap::fromImage(img);

    // Set the QPixmap to the QLabel, scaled to the widget's size, maintaining the aspect ratio
    label->setPixmap(currentPixmap.scaled(label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Update the QLabel to reflect the new frame
    label->update();
}
