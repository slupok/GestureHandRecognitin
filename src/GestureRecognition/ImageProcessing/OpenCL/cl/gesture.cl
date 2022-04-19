
typedef enum PixelType
{
    Undefined,
    Grayscale8,
    Float32,
    RGB24,
    BGR24,
    RGBA32
} PixelType;
typedef uchar Grayscale_format;
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

inline YCC_format RGB2YCC_JPEG(RGB_format rgb)
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

inline int RGB2Grayscale(RGB_format rgb)
{
    float r = rgb.r / 255.0f;
    float g = rgb.g / 255.0f;
    float b = rgb.b / 255.0f;

    float linearColor = 0.2126f * r + 0.7152f * g + 0.0722f * b;
    return (int)(linearColor * 255.0f);
    //float grayscale = linearColor <= 0.0031308 ? 12.92 * linearColor : 1.055 * pow(linearColor, 1/2.4f) - 0.055;
}


//Евклидова метрика
inline float length_E(RGB_format a, RGB_format b)
{
    return sqrt((float)(a.r - b.r)*(a.r - b.r) + (a.g - b.g)*(a.g - b.g) + (a.b - b.b)*(a.b - b.b));
}
//Манхеттена метрика
inline float length_M(RGB_format a, RGB_format b)
{
    return ( abs(a.r - b.r) + abs(a.g - b.g) + abs(a.b - b.b));
}

__kernel void grayscaleColorConversionKernel(const __global RGB_format *image,
                                             __global char *grayImage,
                                             int width,
                                             int height)
{
    int x = get_global_id(0);
    int y = get_global_id(1);

    if(x >= width || y >= height)
        return;
    int index = x + y * width;


    grayImage[index] = RGB2Grayscale(image[index]);

}
__kernel void thresholdColorConversionKernel(const __global RGB_format *image,
                                             __global char* mask,
                                             int width,
                                             int height)
{
    //необходимо учитывать ColorType

    int x = get_global_id(0);
    int y = get_global_id(1);

    if(x >= width || y >= height)
        return;
    int index = x + y * width;


    YCC_format color = RGB2YCC_JPEG(image[index]);

    if(25 <= color.Y && color.Y < 220 &&
       100 <= color.Cb && color.Cb < 130 &&
       140 <= color.Cr && color.Cr < 190)
    {
        mask[index] = 255;
    }
    else
    {
        mask[index] = 0;
    }
}

__kernel void SeparableGaussianBlurKernel(__global void *image,
                                          __constant void *baseImage,
                                          __constant float* filter,
                                          int radius,
                                          int isHorizontal,
                                          int width,
                                          int height,
                                          PixelType pixelType)
{


    int x = get_global_id(0);
    int y = get_global_id(1);

    if(x >= width - radius || y >= height - radius || x < radius || y < radius)
        return;
    int index = x + y * width;

    float3 acc = {0.0f, 0.0f, 0.0f};
    int filterDim = radius * 2 + 1;
    int ix = x, iy = y;
    float weight;


#if 0
    //separable
    for(int i = -radius; i <= radius; i++)
    {
        if(isHorizontal)
            ix = x + i;
        else
            iy = y + i;

        weight = filter[i + radius];
        if(pixelType == RGB24)
        {
            acc.x += weight * (((__constant RGB_format*)baseImage)[ix + width * iy].r / 255.0f);
            acc.y += weight * (((__constant RGB_format*)baseImage)[ix + width * iy].g / 255.0f);
            acc.z += weight * (((__constant RGB_format*)baseImage)[ix + width * iy].b / 255.0f);
        }
    }
#else
    for(int j = -radius; j <= radius; j++)
        for(int i = -radius; i <= radius; i++)
        {
            ix = x + i;
            iy = y + i;
            weight = filter[(i+radius) + filterDim * (j+radius)];
            if(pixelType == RGB24)
            {
                acc.x += weight * (((__constant RGB_format*)baseImage)[ix + width * iy].r / 255.0f);
                acc.y += weight * (((__constant RGB_format*)baseImage)[ix + width * iy].g / 255.0f);
                acc.z += weight * (((__constant RGB_format*)baseImage)[ix + width * iy].b / 255.0f);
            }
        }
#endif

    if(pixelType == RGB24)
    {
        //?
        //((RGB_format*)image)[index] = {(int)(acc.x * 255.0f), (int)(acc.y * 255.0f), (int)(acc.z * 255.0f)};
        ((__global RGB_format*)image)[index].r = (int)(acc.x * 255.0f);
        ((__global RGB_format*)image)[index].g = (int)(acc.y * 255.0f);
        ((__global RGB_format*)image)[index].b = (int)(acc.z * 255.0f);
    }

#if 0
    for(int j = 0; j < radius; j++)
        for(int i = 0; i < radius; i++)
        {
            jy = y + j - r;
            ix = x + i - r;
            weight = filter[i + filterDim * j];
            if(pixelType == RGB24)
            {
                acc.x += weight * ((RGB_format*)baseImage)[ix + width * jy].x
            }
        }
#endif
}

__kernel void MorphologicalErosionKernel(__global uchar *image,
                                         __constant uchar *baseImage,
                                         int radius,
                                         int width,
                                         int height)
{
    int x = get_global_id(0);
    int y = get_global_id(1);

    if(x >= width - radius || y >= height - radius || x < radius || y < radius)
        return;
    int index = x + y * width;

    if(baseImage[index] != 255)
            return;

    for(int iy = -radius; iy <= radius; iy++)
        for(int ix = -radius; ix <= radius; ix++)
        {
            image[(x+ix) + (y+iy) * width] = 255;
        }

}

__kernel void MorphologicalDilationKernel(__global uchar *image,
                                          __constant uchar *baseImage,
                                          int radius,
                                          int width,
                                          int height)
{
    int x = get_global_id(0);
    int y = get_global_id(1);

    if(x >= width - radius || y >= height - radius || x < radius || y < radius)
        return;
    int index = x + y * width;

    if(baseImage[index] != 0)
            return;

    for(int iy = -radius; iy <= radius; iy++)
        for(int ix = -radius; ix <= radius; ix++)
        {
            image[(x+ix) + (y+iy) * width] = 0;
        }
}


/*
typedef struct Cluster{
    RGB_format center;
    int *positions;//хранит индексы
    int pointsCount;
} Cluster;

//detect color belong to a cluster
__kernel void kMeansKernel(const __global RGB_format *image,
                           __global char* clustersMask,
                           __global Cluster *clusters,
                           int clustersCount,
                           int width,
                           int height,
                           _constant long *sum_r,
                           _constant long *sum_g,
                           _constant long *sum_b,
                           int iterationIndex)
{
    int x = get_global_id(0);
    int y = get_global_id(1);

    if(x >= width || y >= height)
        return;
    int ind = x + y * width;

    RGB_format color = image[ind];
    float min;
    int nearestClusterIndex = 0;//Номер ближайшего кластера
    int len;

    sum_r[numNearestCluster] = 0;//prev
    sum_g[numNearestCluster] = 0;//prev
    sum_b[numNearestCluster] = 0;//prev


    for(int n = 0; n < clustersCount; n++)
    {
        len = length_M(color, clusters[n].center);
        if(len < min)
        {
            min = len;
            nearestClusterIndex = n;
        }
    }
    int prevPointsCount =  atomic_inc(&clusters[nearestClusterIndex].pointsCount);
    clusters[nearestClusterIndex].positions[prevPointsCount] = ind;

    clustersMask[ind] = nearestClusterIndex;

    atomic_add(sum_r + nearestClusterIndex, color.r);
    atomic_add(sum_g + nearestClusterIndex, color.g);
    atomic_add(sum_b + nearestClusterIndex, color.b);
}

*/

__kernel void test(__global int* A, __global int *B, __global int *res)
{
    int i = get_global_id(0);
    res[i] = A[i] + B[i];
}
