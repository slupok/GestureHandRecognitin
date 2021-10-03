#include "webcam.h"
#include "ui_webcam.h"

WebCam::WebCam(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WebCam)
{
    ui->setupUi(this);
}

WebCam::~WebCam()
{
    delete ui;
}

void WebCam::SetCamera(QCameraInfo cameraInfo)
{
    WebCam::m_camera = new QCamera(cameraInfo);

    //QCameraViewfinder *viewFinder = new QCameraViewfinder();
    //m_camera->setViewfinder(viewFinder);
    //m_camera->setCaptureMode( QCamera::CaptureViewfinder );

    //ui->gridLayout_2->addWidget(viewFinder,0,0);



    QAbstractVideoSurface *finder = new BasedVideoSurface(ui->myLabel);
    //finder->setCamera(m_camera);
    m_camera->setViewfinder(finder);

    m_camera->start();

}

void WebCam::StartCamera()
{
    m_camera->start();
    m_imageCapture = new QCameraImageCapture(m_camera);
    m_imageCapture->setCaptureDestination( QCameraImageCapture::CaptureToBuffer );
    m_imageCapture->bufferFormatChanged(QVideoFrame::Format_RGB32);

    QImageEncoderSettings imageSettings = m_imageCapture->encodingSettings();
    QSize size = imageSettings.resolution();
    m_cameraWidth = size.width();
    m_cameraHeight = size.height();
}

void WebCam::GetFrame()
{
//как бы получить кадр из видео потока?
    //отнаследоваться от AbstractVideoSurface
#if 1
    QImage img(m_cameraWidth, m_cameraHeight,  QImage::Format_RGB32);
    m_camera->searchAndLock();
    m_imageCapture->imageCaptured(0, img);
    m_camera->unlock();
#endif
    img.save("out.jpg", "JPG");

}

QLabel* WebCam::GetLabel()
{
    return ui->myLabel;
}

