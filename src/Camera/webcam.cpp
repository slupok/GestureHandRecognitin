#include "webcam.h"

WebCam::WebCam(QWidget *parent)
    : QLabel(parent), m_size(0, 0)
{
    setScaledContents(true);
    setFocusPolicy(Qt::StrongFocus);
}

WebCam::~WebCam()
{

}

void WebCam::setImage(QImage img)
{
    setPixmap(QPixmap::fromImage(img));
    update();
}



