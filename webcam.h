#ifndef WEBCAM_H
#define WEBCAM_H

#include <QMainWindow>
#include <QImage>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class WebCam; }
QT_END_NAMESPACE



class WebCam : public QMainWindow
{
    Q_OBJECT

public:
    WebCam(QWidget *parent = nullptr);
    ~WebCam();
    void setImage(QImage img);
private:
    Ui::WebCam *ui;

    QPixmap m_pixmap;
};
#endif // WEBCAM_H
