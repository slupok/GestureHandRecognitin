#include "camera.h"

Camera::Camera()
{
    m_surface = new BasedVideoSurface(this);
    m_image = new QImage();
    //m_buffer = new ImageDataBuffer();
}

void Camera::capture(QImage image)
{
    *m_image = image;
    ImageBuffer buffer;
    newImageBuffer(&buffer,m_image->bits(),m_image->width(),m_image->height(),PixelTypeRGB);
    //m_buffer->newImage(m_image->bits(),m_image->width(),m_image->height(),PixelTypeRGB);
     Callback(&buffer);
}

void Camera::start()
{
    m_camera->start();
}

void Camera::stop()
{
    m_camera->stop();
}

void Camera::setCamera(QCameraInfo cameraInfo)
{
    m_camera = new QCamera(cameraInfo);
     m_camera->setViewfinder(m_surface);
}

Camera::~Camera()
{
    delete m_surface;
    delete m_image;
    delete m_camera;
}
