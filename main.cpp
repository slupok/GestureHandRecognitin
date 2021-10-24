#include "webcam.h"
#include <QApplication>
#include "gestureRecognition.h"
#include "camera.h"

#include <QDebug>
#include <QObject>

#include <CL/cl.h>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WebCam ui;
    ui.show();

    cl_platform_id platform_id = 0x0;
    cl_device_id device_id = 0x0;
    cl_uint num_devices;
    cl_uint num_platforms;
    cl_int ret;


    //test();
    GestureRecognition *rg = new GestureRecognition();
    rg->setUI(&ui);
    rg->startGR();

    return a.exec();
}
