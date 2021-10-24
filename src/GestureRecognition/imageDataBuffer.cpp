#include "string.h"

#include <QImage>

#include "imageDataBuffer.h"


YCC_format RGB2YCC_JPEG(RGB_format rgb)
{
    float r = rgb.r;
    float g = rgb.g;
    float b = rgb.b;
    YCC_format color = {
        0.229f * r + 0.587f * g + 0.114f * b,
        128 - 0.168736f * r - 0.331264f * g + 0.5f * b,
        128 + 0.5f * r - 0.418688f * g + 0.081312f * b
    };
    return color;
}
HSV_format RGB2HSV(RGB_format rgb)
{
    HSV_format HSVcolor;
    float r = rgb.r / 255.0f;
    float g = rgb.g / 255.0f;
    float b = rgb.b / 255.0f;
    float min = r < g ? (r < b ? r : b) : (g < b ? g : b);
    float max = r > g ? (r > b ? r : b) : (g > b ? g : b);

    if(max == r && g >= b)
        HSVcolor.H = 60 * ((g - b)/(max - min));
    else if(max == r && g < b)
        HSVcolor.H = 60 * ((g - b)/(max - min)) + 360;
    else if(max == g)
        HSVcolor.H = 60 * ((b - r)/(max - min)) + 120;
    else if(max == b)
        HSVcolor.H = 60 * ((r - g)/(max - min)) + 240;

    if(max == 0)
        HSVcolor.S = 0.0f;
    else
        HSVcolor.S = 1.0f - min/max;

    HSVcolor.V = max;


    return HSVcolor;
}

RGB_format YCC2RGB_JPEG(YCC_format ycc)
{
    float Y = ycc.Y;
    float Cb = ycc.Cb;
    float Cr = ycc.Cr;
    RGB_format color = {
        (unsigned char)(Y + 1.402f * (Cr - 128)),
        (unsigned char)(Y - 0.34414f * (Cb-128) - 0.71414f * (Cr - 128)),
        (unsigned char)(Y + 1.772f * (Cb - 128))
    };
        return color;
}

int newImageBuffer(ImageBuffer *buffer, const void* bytes, const int width, const int height, const PixelType pixelType)
{
    buffer->imageRgb = (RGB_format*)malloc(width * height * sizeof(RGB_format));
    buffer->imageYCC = (YCC_format*)malloc(width * height * sizeof(YCC_format));
    buffer->imageHSV = (HSV_format*)malloc(width * height * sizeof(HSV_format));
    buffer->bitmap = (BIT_MAP_TYPE*)malloc(width * height * sizeof(BIT_MAP_TYPE));
    if(pixelType == PixelTypeRGB)//RGB24
    {
        buffer->type = PixelTypeRGB;
        buffer->width = width;
        buffer->height = height;
        buffer->bytes = (void*)malloc(width * height * sizeof(RGB_format));
        memcpy(buffer->bytes, bytes, width * height * sizeof(RGB_format));
        bufferToAllFormat(buffer,bytes,pixelType);
        return 1;
    }
    return 0;
}
int bufferToAllFormat(ImageBuffer *buffer, const void *bytes, const PixelType pixelType)
{
    if(pixelType == PixelTypeRGB)
    {
        memcpy(buffer->imageRgb,bytes,buffer->height * buffer->width * sizeof (RGB_format));
        int i = 0;
        for(int y = 0; y < buffer->height; y++)
            for(int x = 0; x < buffer->width; x++)
            {
                i = x + buffer->width * y;
                buffer->imageYCC[i] = RGB2YCC_JPEG(buffer->imageRgb[i]);
                buffer->imageHSV[i] = RGB2HSV(buffer->imageRgb[i]);
            }
        return 1;
    }
    else if (pixelType == PixelTypeYCC)
    {
        memcpy(buffer->imageYCC,bytes,buffer->height * buffer->width * sizeof (YCC_format));
        int i = 0;
        for(int y = 0; y < buffer->height; y++)
            for(int x = 0; x < buffer->width; x++)
            {
                i = x + buffer->width * y;
                buffer->imageRgb[i] = YCC2RGB_JPEG(buffer->imageYCC[i]);
            }
        return 1;
    }
    return 0;
}

void releaseImageBuffer(ImageBuffer *buffer)
{
    if(!buffer)
        return;
    if(buffer->bitmap)
        free(buffer->bitmap);
    if(buffer->bytes)
        free(buffer->bytes);
    if(buffer->imageRgb)
        free(buffer->imageRgb);
    if(buffer->imageYCC)
        free(buffer->imageYCC);
}

