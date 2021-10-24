#ifndef GESTURERECOGNITION_H
#define GESTURERECOGNITION_H
#include <math.h>

#include "imageDataBuffer.h"

#include "../Camera/camera.h"
#include "../Camera/webcam.h"


//void thresholdColorConversion(ImageBuffer buffer);
void test();
class GestureRecognition
{

public:
    GestureRecognition();
    ~GestureRecognition();
    static void thresholdColorConversion(ImageBuffer *buffer);
    void startGR();
    void setUI(WebCam *ui);
private:
    Camera *m_camera;

};
#endif // GESTURERECOGNITION_H
