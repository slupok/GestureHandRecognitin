 #ifndef CAMERA_H
#define CAMERA_H

#include <QCamera>
#include <QCameraInfo>

#include "videoSurfaceCaptured.h"
#include "basedVideoSurface.h"
#include "../GestureRecognition/imageDataBuffer.h"

class GestureRecognition;

class CameraHandler
{
public:
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void setCamera(QCameraInfo cameraInfo) = 0;
    std::function<void (ImageBuffer *buffer)> Callback ;

};

class Camera : VideoSurfaceCaptured, public CameraHandler
{
public:
    Camera();
    ~Camera();

    void capture(QImage);
    void start();
    void stop();
    void setCamera(QCameraInfo cameraInfo);

private:
    QImage *m_image;
    QCamera *m_camera;
    BasedVideoSurface *m_surface;
    //ImageBuffer *m_buffer;

};

#endif // CAMERA_H
