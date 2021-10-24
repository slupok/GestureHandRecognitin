#ifndef IMAGEDATABUFFER_H
#define IMAGEDATABUFFER_H
#include <stdlib.h>

//эта структура для обработки изображение, переместить в алгоритм
typedef enum PixelType{
    PixelTypeNone,
    PixelTypeRGB,
    PixelTypeYCC,
    PixelTypeBinarized
} ;
typedef char BIT_MAP_TYPE;
typedef struct RGB_format
{
  unsigned char r,g,b;
} RGB_format;

typedef struct YCC_format
{
    float Y, Cb, Cr;
}YCC_format;
typedef struct HSV_format
{
 float H,S,V;
}HSV_format;

YCC_format RGB2YCC_JPEG(RGB_format rgb);
HSV_format RGB2HSV(RGB_format rgb);

RGB_format YCC2RGB_JPEG(YCC_format ycc);

typedef struct ImageBuffer
{
    void* bytes;
    RGB_format *imageRgb;
    YCC_format *imageYCC;
    HSV_format *imageHSV;
    char *bitmap;
    int width, height;
    PixelType type;
} ImageBuffer;

int newImageBuffer(ImageBuffer *buffer, const void* bytes, const int width, const int height, const PixelType pixelType);
int bufferToAllFormat(ImageBuffer *buffer, const void *bytes, const PixelType pixelType);
void releaseImageBuffer(ImageBuffer *buffer);
#endif // IMAGEDATABUFFER_H
