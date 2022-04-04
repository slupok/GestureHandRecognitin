#ifndef GESTURERECOGNITION_H
#define GESTURERECOGNITION_H
#include <math.h>


#include "imageDataBuffer.h"
#include "ImageProcessing/imageProcessing.h"
#include "../Camera/camera.h"
#include "../Camera/webcam.h"

#include "ImageProcessing/OpenCL/opencl_context.h"


class GestureRecognition : public QObject
{
    Q_OBJECT
public:
    GestureRecognition();
    ~GestureRecognition();
    static void thresholdColorConversion(ImageBuffer *buffer);
    void startGR();
    void setUI(WebCam *ui);


public slots:
    void onUpdateFrame(QImage frame);

private:
    WebCam *m_ui;
    Camera *m_camera;
    IPContext *m_context;

    IPImage *m_image;
    IPImage *m_backgroundMask;
    IPImage *m_mask;

};
#endif // GESTURERECOGNITION_H
