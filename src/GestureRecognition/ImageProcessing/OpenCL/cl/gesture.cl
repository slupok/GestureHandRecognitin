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


__kernel void thresholdColorConversionKernel(const __global RGB_format *image,
                                             __global char* mask,
                                             int width,
                                             int height)
{
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
