TARGET = GestureHandRecognition

QT       += core gui multimedia multimediawidgets

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimediawidgets

CONFIG += stl c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/GestureRecognition/gestureRecognition.cpp \
 #   src/GestureRecognition/imageDataBuffer.cpp \
    src/Camera/camera.cpp \
    src/Camera/basedVideoSurface.cpp \
    src/Camera/webcam.cpp

HEADERS += \
    src/GestureRecognition/gestureRecognition.h \
   # src/GestureRecognition/imageDataBuffer.h \
    src/Camera/camera.h \
    src/Camera/videoSurfaceCaptured.h \
    src/Camera/basedVideoSurface.h \
    src/Camera/webcam.h

#image processing
SOURCES += \
    src/GestureRecognition/ImageProcessing/ipContext.cpp \
    src/GestureRecognition/ImageProcessing/ipImage.cpp \
    src/GestureRecognition/ImageProcessing/ipDevice.cpp
HEADERS += \
    src/GestureRecognition/ImageProcessing/imageProcessing.h \
    src/GestureRecognition/ImageProcessing/ipContext.h \
    src/GestureRecognition/ImageProcessing/ipImage.h \
    src/GestureRecognition/ImageProcessing/ipDevice.h

#image processing openCL
SOURCES += \
  #  src/GestureRecognition/ImageProcessing/OpenCL/opencl_processing.cpp \
    src/GestureRecognition/ImageProcessing/OpenCL/opencl_device.cpp \
  #  src/GestureRecognition/ImageProcessing/OpenCL/opencl_program.cpp \
    src/GestureRecognition/ImageProcessing/OpenCL/opencl_image.cpp \
    src/GestureRecognition/ImageProcessing/OpenCL/opencl_context.cpp

HEADERS += \
    src/GestureRecognition/ImageProcessing/ipOpenCL.h \
    src/GestureRecognition/ImageProcessing/OpenCL/opencl_device.h \
 #   src/GestureRecognition/ImageProcessing/OpenCL/opencl_program.h \
    src/GestureRecognition/ImageProcessing/OpenCL/opencl_image.h \
    src/GestureRecognition/ImageProcessing/OpenCL/opencl_context.h

OTHER_FILES += \
      src/GestureRecognition/ImageProcessing/OpenCL/cl/gesture.cl

FORMS += \
    src/Camera/webcam.ui


OPENCL_PATH = "C://Program Files (x86)//AMD APP SDK//3.0"

LIBS += -L $${OPENCL_PATH}/lib/x86_64 -lOpenCL
INCLUDEPATH += $${OPENCL_PATH}/include/
DEPENDPATH += $${OPENCL_PATH}/include/

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target
