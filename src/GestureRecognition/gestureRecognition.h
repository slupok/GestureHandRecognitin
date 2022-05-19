#ifndef GESTURERECOGNITION_H
#define GESTURERECOGNITION_H
#include <math.h>
#include <QFile>

#include "imageDataBuffer.h"
#include "ImageProcessing/imageProcessing.h"
#include "../Camera/camera.h"
#include "../Camera/webcam.h"

#include "ImageProcessing/OpenCL/opencl_context.h"

#define GESTURE_COUNT 2
#define INVARIANTS_COUNT 7
#define MOMENTS_COUNT 6

#define FIVE_INV 0
#define NICE_INV 1

class GestureRecognition : public QObject
{
    Q_OBJECT
public:
    GestureRecognition();
    ~GestureRecognition();
    static void thresholdColorConversion(ImageBuffer *buffer);
    void startGR();
    void setUI(WebCam *ui);

public slots:
    void onUpdateFrame(QImage frame);

private:
    float NormalizedCentralMoment(int p, int q, int cx, int cy, int m00);

private:
    WebCam *m_ui;
    Camera *m_camera;
    IPContext *m_context;

    bool m_block;
    int cntFrame = 0;//костыль
    IPImage *m_image;
    IPImage *m_backgroundMask;
    IPImage *m_mask;
    IPImage *m_tmpMask;

    //[количество жестов] [инварианты(I1 - I7)] [кол-во моментов]
    float m_DataInv[GESTURE_COUNT][INVARIANTS_COUNT][MOMENTS_COUNT] = {
        {//FIVE
            {0.0739795f,   0.0884475f,  0.0760744f,    0.0770494f,    0.0768502f,   0.0796616},
            {0.000135335f, 0.00026481f, 0.000133793f,  0.000287869f,  0.000295291f, 0.000313737f},
            {0.0146921f,   0.0189576f,  0.0150057f,    0.0217647f,    0.0227824f,   0.0246011f},
            {0.0146921f,   0.0189576f,  0.0150057f,    0.0217647f,    0.0227824f,   0.0246011f},
            {0.000215856f, 0.00035939f, 0.000225171f,  0.000473703f,  0.000519039f, 0.000605216f},
            {0.000165971f, 0.00028693f, 0.000168989f,  0.00035545f,   0.000374913f, 0.000415665f},
            {0.000113564f, 0.000182403f, 0.000117723f, 0.000273029f,  0.000301862f, 0.000352487f}
        },

        {//NICE
            {0.0895471f,  0.0928822f,  0.0909935f,  0.116136f,   0.110502f,   0.108428f},
            {0.00222431f, 0.00215048f, 0.00231714f, 0.0035011f,  0.00307571f, 0.0031716f},
            {0.122154f,   0.148594f,   0.124779f,   0.0722959f,  0.0609849f,  0.0584049f},
            {0.122153f,   0.148594f,   0.124779f,   0.0722959f,  0.0609849f,  0.0584049f},
            {0.0149215f,  0.0220802f,  0.0155698f,  0.0052267f,  0.00371915f, 0.00341113f},
            {0.00574807f, 0.00689062f, 0.00596922f, 0.00375879f, 0.00304579f, 0.00300428f},
            {0.00979165f, 0.0146727f,  0.010176f,   0.003316f,   0.00232937f, 0.00213167f}
        }
    };

    float m_fiveInv[7];
    float m_niceInv[7];

};
#endif // GESTURERECOGNITION_H
