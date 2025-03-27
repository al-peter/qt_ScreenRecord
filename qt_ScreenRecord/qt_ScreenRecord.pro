QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 console
CONFIG -= app_bundle

INCLUDEPATH += /usr/local/include/opencv4/
INCLUDEPATH += ~/rknn_toolkit2/include/
INCLUDEPATH += /home/orangepi/Documents/Projects/qt_ScreenRecord/qt_ScreenRecord/include/

LIBS += -L/usr/local/lib \
    -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio \
    -lopencv_dnn -lopencv_objdetect -lopencv_imgcodecs
LIBS += -L~/rknn_toolkit2/lib -lrknn_api

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        mainwindow.cpp \
        postporcess.cpp \
        rkYolov5s.cpp \
        screenrecorderthread.cpp \
        videocapturethread.cpp \
        videowidget.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    mainwindow.h \
    screenrecorderthread.h \
    videocapturethread.h \
    videowidget.h
