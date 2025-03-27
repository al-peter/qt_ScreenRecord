# Screen Recording and Object Detection with YoloV5 for Orange Pi 5

This project captures video from a camera, processes it using the YoloV5 model for object detection, and supports screen and window recording. It is designed for the Orange Pi 5 platform, using **Qt**, **C++**, and **OpenCV** for video capture and processing. The YoloV5 model is utilized for object recognition, and the project integrates with **FFmpeg** for screen recording.

## Features

- **Video Capture**: Capture video from a camera using OpenCV and display it in a Qt window.
- **FPS Display**: Display the current frames per second (FPS) of the video capture.
- **Window and Screen Recording**: Record the screen or a specific window using FFmpeg with the ability to save the recording as a `.mp4` file.
- **Object Detection**: Use the YoloV5 model (converted for use with the RK3588 NPU) for object detection on the captured video stream.
- **UDP Streaming**: Send captured frames over UDP for real-time processing.

## Requirements

- **Orange Pi 5**: The project is optimized for the Orange Pi 5 platform.
- **Qt 5**: The GUI is built with Qt.
- **OpenCV 4-5-0**: For video capture and processing.
- **FFmpeg**: For screen recording functionality.
- **YoloV5 Model**: The YoloV5 model, converted to run on the RK3588 NPU, is used for object detection.

