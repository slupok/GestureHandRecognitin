#include <math.h>

#include "gestureRecognition.h"

//Евклидова метрика
float length_E(RGB_format a, RGB_format b)
{
    return sqrt((a.r - b.r)*(a.r - b.r) + (a.g - b.g)*(a.g - b.g) + (a.b - b.b)*(a.b - b.b));
}
//Манхеттена метрика
float length_M(RGB_format a, RGB_format b)
{
    return ( abs(a.r - b.r) + abs(a.g - b.g) + abs(a.b - b.b));
}

void setMaskForSkinColorCluster(unsigned char *mask, const QImage image)
{
    int width = image.width();
    int height = image.height();
    QRgb rgb;

    HSV_format min_HSV = {0.0f,45.0f/255.0f,0.0f};
    HSV_format max_HSV = {20.0f,255.0f/255.0f,255.0f/255.0f};

    YCC_format YCCcolor;
    RGB_format RGBcolor;
    HSV_format HSVcolor;
    int i = 0;
    for(int y = 0; y < height; y++)
        for(int x = 0; x < width; x++)
        {
            i = y * width + x;



            rgb = image.pixel(x,y);
            RGBcolor = {(unsigned char)qRed(rgb),
                        (unsigned char)qGreen(rgb),
                        (unsigned char)qBlue(rgb)};
            YCCcolor = RGB2YCC_JPEG(RGBcolor);
            HSVcolor = RGB2HSV(RGBcolor);


            if(min_HSV.H <= HSVcolor.H && HSVcolor.H <= max_HSV.H &&
               min_HSV.S <= HSVcolor.S && HSVcolor.S <= max_HSV.S &&
               min_HSV.V <= HSVcolor.V && HSVcolor.V <= max_HSV.V)
            {
                mask[i] = 1;
            }
            else
                mask[i] = 0;


#if 0
            //цветовой кластер кожи человека
            if(25 <= YCCcolor.Y && YCCcolor.Y < 220
                    && 100 <= YCCcolor.Cb && YCCcolor.Cb < 130
                    && 140 <= YCCcolor.Cr && YCCcolor.Cr < 190)
            {
                mask[i] = 1;
            }
            else
            {
                mask[i] = 0;
            }
#endif

        }
}

typedef struct Cluster{
    RGB_format center;
    int *positions;//хранит индексы
    int pointsCount;
} Cluster;

//поиск принадлежности можно вычислять как ближайшую точку к кластерам
//а не перебирать по всем точкам
bool isPositionInCluster(int index, Cluster cluster)
{
    for(int i = 0; i < cluster.pointsCount; i++)
    {
        if(index == cluster.positions[i])
            return true;
    }

    return false;
}

uint getNumberClusterWithColor(Cluster* clusters, int cntClusters, RGB_format color)
{
    float min = length_M(color,clusters[0].center);
    uint numNearestCluster = 0;
    int len;
    for(int i = 1; i < cntClusters; i++)
    {
        len = length_M(color,clusters[i].center);
        if(len < min)
        {
            min = len;
            numNearestCluster = i;
        }
    }

    return 0;
}

void Kmeans(unsigned char *mask, RGB_format *image, int height, int width)
{
    /************
     * init
     ************/
    int clustersCount = 5;
    if(clustersCount <= 0)
        return;
    Cluster* cluster = (Cluster*)malloc(sizeof (Cluster) * width * height * clustersCount);
    int step = 255/5;
    for(int i = 0; i < clustersCount; i++ )
    {
        cluster[i].positions = (int*)malloc(sizeof (int) * width * height);
        cluster[i].pointsCount = 0;
        cluster[i].center = {(unsigned char)(i*step),
                             (unsigned char)(i*step),
                             (unsigned char)(i*step)};
    }

    /************
     * k iteration
     ************/
    int iterationCount = 40;
    RGB_format color;
    float min;
    int numNearestCluster;//Номер ближайшего кластера
    int len;
    long *sum_r = (long*)malloc(sizeof (long) * clustersCount);
    long *sum_g = (long*)malloc(sizeof (long) * clustersCount);
    long *sum_b = (long*)malloc(sizeof (long) * clustersCount);
    for(int k = 0; k < iterationCount; k++)
    {
        /************
         * определение принадлежности цвета к кластерам
         ************/

        sum_r[numNearestCluster] = 0;
        sum_g[numNearestCluster] = 0;
        sum_b[numNearestCluster] = 0;

        for(int i = 0; i < width * height ; i++)
        {
            color = image[i];
            min = length_M(color,cluster[0].center);
            numNearestCluster = 0;
            for(int n = 0; n < clustersCount; n++)
            {
                len = length_M(color,cluster[n].center);
                if(len < min)
                {
                    min = len;
                    numNearestCluster = n;
                }
            }
            cluster[numNearestCluster].positions[cluster[numNearestCluster].pointsCount++] = i;

            mask[i] = numNearestCluster;

            sum_r[numNearestCluster] += color.r;
            sum_g[numNearestCluster] += color.g;
            sum_b[numNearestCluster] += color.b;
        }

        //перерасчет центров
        for(int n = 0; n < clustersCount; n++)
        {
            int t = cluster[n].pointsCount;
            cluster[n].pointsCount = 0;
            if(t == 0)
                continue;
            cluster[n].center = {(unsigned char)(sum_r[n]/t),(unsigned char)(sum_g[n]/t),(unsigned char)(sum_b[n]/t)};
        }
    }

    free(sum_r);
    free(sum_g);
    free(sum_b);
    for(int n = 0; n < clustersCount; n++)
    {
        free(cluster[n].positions);
    }
    free(cluster);

}

void setBinarizedImage(uint8_t *mask, QImage *img)
{
    int i = 0;
    for(int y = 0; y < img->height(); y++)
        for(int x = 0; x < img->width(); x++)
        {
            i = y*img->width() + x;
            if(mask[i] == 0)
                img->setPixel(x,y,0x000000);
            if(mask[i] == 1)
                img->setPixel(x,y,0x444444);
            if(mask[i] == 2)
                img->setPixel(x,y,0x888888);
            if(mask[i] == 3)
                img->setPixel(x,y,0xCCCCCC);
            if(mask[i] == 4)
                img->setPixel(x,y,0xFFFFFF);
        }
}

void GestureRecognition::thresholdColorConversion(ImageBuffer *buffer)
{

#if 0

    QImage img((unsigned char*)buffer->image,buffer->width,buffer->height,QImage::Format_RGB888);

    unsigned char *mask = (unsigned char*)malloc(sizeof(unsigned char) * buffer->width * buffer->height);

    //можно посмотреть другие форматы изображений
    QImage outImage(buffer->width,buffer->height,QImage::Format_RGB32);
    //setMaskForSkinColorCluster(mask,img);
    Kmeans(mask,buffer->image,buffer->width,buffer->height);
    setBinarizedImage(mask, &outImage);
        m_ui->setImage(outImage);
    outImage.save("out.jpg", "JPG");

    free(mask);
#endif
}

GestureRecognition::GestureRecognition() : QObject()
{
    m_context = new OpenclContext();
    m_camera = nullptr;
    m_image = nullptr;
    m_backgroundMask = nullptr;
    m_mask = nullptr;

}
GestureRecognition::~GestureRecognition()
{
    if(m_context)
        delete m_context;
    m_context = nullptr;

    if(m_camera)
        delete m_camera;
    m_camera = nullptr;
}
void GestureRecognition::startGR()
{
    QCameraInfo camerainfo = QCameraInfo::defaultCamera();
    m_camera = new Camera();
    m_camera->setCamera(camerainfo);
    m_camera->start();
    connect(m_camera, &CameraHandler::callbackFrame, this, &GestureRecognition::onUpdateFrame);

    //init IPImage with width, height and PixelData
}

void GestureRecognition::onUpdateFrame(QImage frame)
{
    IPError error;

    int width = frame.width();
    int height = frame.height();
    if(!m_image)
    {
        // first frame
        m_image = m_context->CreateImage(width, height, PixelType::RGB24, (unsigned char*)frame.bits());
        m_backgroundMask = m_context->CreateImage(width, height, PixelType::Grayscale8, 0x0);
        m_mask = m_context->CreateImage(width, height, PixelType::Grayscale8, 0x0);
    }
    else
    {
        m_image->WriteImage((unsigned char*)frame.bits(), width * height * GetBytesPerPixel(PixelType::RGB24));
    }
#if 1


    error = m_context->ColorThresholdConversion(m_image, m_mask);
    if(error != IPNoError)
        qDebug() << "ColorThresholdConversion error";
    //...


    //m_image->ReadImage(frame.bits(), width * height * GetBytesPerPixel(PixelType::RGB24));
    if(m_mask->GetPixelType() == PixelType::Grayscale8)
    {
        QImage img(width, height, QImage::Format_Grayscale8);
        m_mask->ReadImage(img.bits(), width * height * GetBytesPerPixel(PixelType::Grayscale8));
        m_ui->setImage(img);
    }
#endif

    //m_ui->setImage(frame);
}

void GestureRecognition::setUI(WebCam *ui)
{
    m_ui = ui;
}
