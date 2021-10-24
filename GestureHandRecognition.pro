QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimediawidgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    basedVideoSurface.cpp \
    camera.cpp \
    gestureRecognition.cpp \
    imageDataBuffer.cpp \
    main.cpp \
    webcam.cpp

HEADERS += \
    camera.h \
    gestureRecognition.h \
    imageDataBuffer.h \
    videoSurfaceCaptured.h \
    basedVideoSurface.h \
    webcam.h

FORMS += \
    webcam.ui


LIBS += -L "C:/Program Files (x86)/AMD APP SDK/2.9-1/lib/x86_64"
INCLUDEPATH += "C:/Program Files (x86)/AMD APP SDK/2.9-1/include/"
DEPENDPATH += "C:/Program Files (x86)/AMD APP SDK/2.9-1/include/"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
