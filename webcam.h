#ifndef WEBCAM_H
#define WEBCAM_H

#include <QMainWindow>
#include <QMultimedia>
#include <QCamera>
#include <QtMultimediaWidgets/QCameraViewfinder>
#include <QCameraInfo>
#include <QCameraImageCapture>

#include <QVideoFrame>
#include <QAbstractVideoSurface>
#include <QLabel>
#include <QVideoSurfaceFormat>

#include <QImageEncoderSettings>

#include "basedVideoSurface.h"
QT_BEGIN_NAMESPACE
namespace Ui { class WebCam; }
QT_END_NAMESPACE



class WebCam : public QMainWindow
{
    Q_OBJECT

public:
    WebCam(QWidget *parent = nullptr);
    ~WebCam();
    void SetCamera(QCameraInfo cameraInfo);
    void StartCamera();
    void GetFrame();
    QLabel* GetLabel();
private:
        Ui::WebCam *ui;

    QCamera *m_camera;
    QCamera *m_bitmapCamera;
    QString m_cameraName;
    int m_cameraWidth;
    int m_cameraHeight;

    QCameraImageCapture *m_imageCapture;
     QCameraImageCapture *m_imageCaptureBitmap;

    QPixmap m_pixmap;
};
#endif // WEBCAM_H
