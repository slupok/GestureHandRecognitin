#include "webcam.h"
#include "ui_webcam.h"

WebCam::WebCam(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WebCam)
{
    ui->setupUi(this);
    ui->label2->setScaledContents(true);
    ui->label->setScaledContents(true);
}

WebCam::~WebCam()
{
    delete ui;
}

void WebCam::setImage(QImage img)
{
    //ui->label->resize(img.size());
    ui->label->setPixmap(QPixmap::fromImage(img));
    //ui->label->setScaledContents(true);
    ui->label->update();
}

void WebCam::setTmpImage(QImage img)
{
    //ui->label->resize(img.size());
    ui->label2->setPixmap(QPixmap::fromImage(img));
    //ui->label_tmp->setScaledContents(true);
    ui->label2->update();
}

