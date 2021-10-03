#ifndef BASEDVIDEOSURFACE_H
#define BASEDVIDEOSURFACE_H

#include <QDebug>
#include <QVideoFrame>
#include <QAbstractVideoSurface>
#include <QLabel>
#include <QVideoSurfaceFormat>

class BasedVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
private:
    QVideoFrame _frame;
    QImage::Format _imageFormat;
    QLabel *_label;


public:
    BasedVideoSurface(QLabel *label)
    {
        _label = label;
    }
    QList<QVideoFrame::PixelFormat> supportedPixelFormats( QAbstractVideoBuffer::HandleType handleType) const
    {
        if (handleType == QAbstractVideoBuffer::NoHandle) {
            return QList<QVideoFrame::PixelFormat>()
                    << QVideoFrame::Format_RGB32
                    << QVideoFrame::Format_ARGB32
                    << QVideoFrame::Format_ARGB32_Premultiplied
                    << QVideoFrame::Format_RGB565
                    << QVideoFrame::Format_RGB555
                    << QVideoFrame::Format_UYVY; //новая строка
        } else {
            return QList<QVideoFrame::PixelFormat>();
        }
    }

    bool present(const QVideoFrame &frame)
    {

        QVideoFrame frametodraw(frame);

        frametodraw.map(QAbstractVideoBuffer::ReadOnly);
                 //this is a shallow operation. it just refer the frame buffer
                 QImage image(
                        frametodraw.bits(),
                        frametodraw.width(),
                        frametodraw.height(),
                        //frametodraw.bytesPerLine(),
                        QVideoFrame::imageFormatFromPixelFormat(frametodraw.pixelFormat()));
                 image = image.convertToFormat(QImage::Format_RGB888);
                 if (surfaceFormat().scanLineDirection() == QVideoSurfaceFormat::BottomToTop)
                     image = image.transformed(QTransform().scale(-1,-1).translate(0, image.width()));


                _label->resize(image.size());

                _label->setPixmap(QPixmap::fromImage(image));
                frametodraw.unmap();

                _label->update();

        return true;
    }

};

#endif // BASEDVIDEOSURFACE_H
