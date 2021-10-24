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

void WebCam::setImage(QImage img)
{
    ui->label->resize(img.size());
    ui->label->setPixmap(QPixmap::fromImage(img));

    ui->label->update();
}



