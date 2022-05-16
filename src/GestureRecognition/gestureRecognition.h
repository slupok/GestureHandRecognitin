#ifndef GESTURERECOGNITION_H
#define GESTURERECOGNITION_H
#include <math.h>
#include <QFile>

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
    float NormalizedCentralMoment(int p, int q, int cx, int cy, int m00);

private:
    WebCam *m_ui;
    Camera *m_camera;
    IPContext *m_context;

    bool m_block;
    int cntFrame = 0;//костыль
    IPImage *m_image;
    IPImage *m_backgroundMask;
    IPImage *m_mask;
    IPImage *m_tmpMask;

};
#endif // GESTURERECOGNITION_H
