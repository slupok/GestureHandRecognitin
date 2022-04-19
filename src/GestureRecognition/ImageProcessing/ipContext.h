#ifndef IP_CONTEXT_H
#define IP_CONTEXT_H

#include "imageProcessing.h"
#include "ipDevice.h"
#include "ipImage.h"
#include "../imageDataBuffer.h" //del

#include "OpenCL/opencl_device.h"

#include "math.h"


struct IPDevices{
    int openCL;
    int CUDA;
    int Metal;
};


class IPContext
{
public:
    IPContext();
    virtual ~IPContext();

    virtual void RunTest() = 0;

    virtual IPImage* CreateImage(uint w, uint h, PixelType pixelType, void *data) = 0;// create IPImage with used device type
#if 0
    virtual IPError AttachImage(IPImage *image) = 0;//можно избавиться, если параметром будет передаваться
    void DetachImage();
    bool IsValidImage();
#endif
    virtual IPEnum GetDeviceType() = 0;

    //functions
    virtual IPError kMeans(int clustersCount, int iterationsCount, RGB_format recognitionColor) = 0;
    virtual IPError ColorThresholdConversion(IPImage *image, IPImage *resultMask) = 0;
    virtual IPError GaussianBlur(IPImage *image, int radius, float sigma) = 0;
    virtual IPError MorphologicalDilation(IPImage *image, int radius) = 0;
    virtual IPError MorphologicalErosion(IPImage *image, int radius) = 0;
//protected:
  //  IPImage *m_image;
};

static const IPDevices SupportedDevices = {
    #ifdef USE_OPENCL
    1,
    #else
    0,
    #endif
    #ifdef USE_CUDA
    1,
    #else
    0,
    #endif
    #ifdef USE_METAL
    1
    #else
    0
    #endif
};

#endif
