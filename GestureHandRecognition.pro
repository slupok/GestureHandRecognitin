QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimediawidgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/Camera/basedVideoSurface.cpp \
    src/Camera/camera.cpp \
    src/GestureRecognition/gestureRecognition.cpp \
    src/GestureRecognition/imageDataBuffer.cpp \
    src/main.cpp \
    src/Camera/webcam.cpp

HEADERS += \
    src/Camera/camera.h \
    src/GestureRecognition/gestureRecognition.h \
    src/GestureRecognition/imageDataBuffer.h \
    src/Camera/videoSurfaceCaptured.h \
    src/Camera/basedVideoSurface.h \
    src/Camera/webcam.h

FORMS += \
    src/Camera/webcam.ui


LIBS += -L "C:/Program Files (x86)/AMD APP SDK/2.9-1/lib/x86_64"
INCLUDEPATH += "C:/Program Files (x86)/AMD APP SDK/2.9-1/include/"
DEPENDPATH += "C:/Program Files (x86)/AMD APP SDK/2.9-1/include/"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
