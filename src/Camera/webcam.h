#ifndef WEBCAM_H
#define WEBCAM_H

#include <QMainWindow>
#include <QImage>
#include <QLabel>


//ImageView
class WebCam : public QLabel
{
    Q_OBJECT

public:
    WebCam(QWidget *parent = nullptr);
    ~WebCam();

    void setImage(QImage img);

private:
    QPixmap m_pixmap;
    QSize m_size;
};
#endif // WEBCAM_H

#if 0
#include <QLabel>
#include <QMatrix4x4>
#include "RayTrace.h"

class SceneWidget : public QLabel
{
public:
    explicit SceneWidget(QWidget *parent = nullptr);
    void setRayTrace(RayTrace *rt);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void setImage(const QPixmap &pixmap);

private:
    bool m_isPressed;
    QPoint m_lastPressedPos;

    RayTrace *m_rt;
};
#endif
