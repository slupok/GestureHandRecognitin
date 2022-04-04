
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
    WebCam ui;
    ui.show();

    //покрыть тестами!!!
#if 0
    IPContext *ctx = new OpenclContext();
    if( ctx->GetDeviceType() == IPDeviceOpenCL)
        qDebug() << "Select OpenCL device";
    ctx->RunTest();
#endif

    GestureRecognition *rg = new GestureRecognition();
    rg->setUI(&ui);
    rg->startGR();

    return a.exec();
}
