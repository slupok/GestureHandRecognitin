#ifndef GESTURERECOGNITION_H
#define GESTURERECOGNITION_H
#include "imageDataBuffer.h"
#include "camera.h"
#include "webcam.h"
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
