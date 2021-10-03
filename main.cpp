#include "webcam.h"
#include <QApplication>
#include "basedViewFinder.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WebCam ui;
    ui.show();
    QCameraInfo camera = QCameraInfo::defaultCamera();
    ui.SetCamera(camera);
    //ui.StartCamera();



    //ui.GetFrame();
    return a.exec();
}
