
#include <QApplication>

#include "Camera/webcam.h"
#include "GestureRecognition/gestureRecognition.h"
#include "Camera/camera.h"

#include <QDebug>
#include <QObject>
#include <QFile>

#include "GestureRecognition/ImageProcessing/imageProcessing.h"

#include "GestureRecognition/ImageProcessing/ipContext.h"
#include "GestureRecognition/ImageProcessing/OpenCL/opencl_context.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QWidget mainWidget;
    mainWidget.resize(800, 600);

    WebCam *baseImage = new WebCam(&mainWidget);
    baseImage->setGeometry(0,0, mainWidget.size().width(), mainWidget.size().height());
    baseImage->setObjectName(QString::fromUtf8("scene"));
    baseImage->setAutoFillBackground(true);


    mainWidget.setWindowTitle(QObject::tr("Gesture Hand Recognition"));
    mainWidget.show();

    GestureRecognition *rg = new GestureRecognition();
    rg->setUI(baseImage);
    rg->startGR();

    return a.exec();
}
