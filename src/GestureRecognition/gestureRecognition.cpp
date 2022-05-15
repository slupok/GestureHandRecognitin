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


}

GestureRecognition::GestureRecognition() : QObject()
{
    m_context = new OpenclContext();
    m_camera = nullptr;
    m_image = nullptr;
    m_backgroundMask = nullptr;
    m_mask = nullptr;
    m_block = false;

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
static bool m_block_ = false;
void GestureRecognition::onUpdateFrame(QImage frame)
{
    if(m_block_)
        return;
    m_block_ = true;

    IPError error;

    int width = frame.width();
    int height = frame.height();
    if(cntFrame <= 70 )
    {
        cntFrame++;
        m_block_ = false;
        return;
    }
    if(!m_image)
    {
        // first frame
        m_image = m_context->CreateImage(width, height, PixelType::RGB24, (unsigned char*)frame.bits());
        m_backgroundMask = m_context->CreateImage(width, height, PixelType::RGB24, (unsigned char*)frame.bits());
        error = m_context->GaussianBlur(m_backgroundMask, 2, 4.0f);
        m_mask = m_context->CreateImage(width, height, PixelType::Grayscale8, 0x0);
        m_tmpMask = m_context->CreateImage(width, height, PixelType::Grayscale8, 0x0);
    }
    else
    {
        m_image->WriteImage((unsigned char*)frame.bits(), width * height * GetBytesPerPixel(PixelType::RGB24));
    }

    uint sum = 0;

    error = m_context->GaussianBlur(m_image, 2, 4.0f);
    error = m_context->FrameDifference(m_image, m_backgroundMask, m_mask, 7);
    error = m_context->MorphologicalErosion(m_mask, 3);
    error = m_context->MorphologicalDilation(m_mask, 2);
    error = m_context->ColorThresholdConversion(m_image, m_mask);

#if 1
    //сохраняем Bitmap
    error = m_context->copyImage(m_mask, m_tmpMask);
    error = m_context->MorphologicalErosion(m_mask, 6);
    error = m_context->MorphologicalDilation(m_mask, 8);
    //находим пересечение новой картинки со старой
    error = m_context->BitmapIntersection(m_mask, m_tmpMask);
#endif

   error = m_context->copyImage(m_mask, m_tmpMask);
   error = m_context->MorphologicalDilation(m_mask, 2);
   error = m_context->BitmapSubtraction(m_mask, m_tmpMask);

   int centerX = 0;
   int centerY = 0;
   error = m_context->CenterOfBitmap(m_mask, centerX, centerY);
   int m00 = 0;
   error = m_context->CentralMoment(m_image, m_mask, 0, 0, 0, 0, m00);

   int p, q;

   int m20 = 0;
   p = 2;
   q = 0;
   error = m_context->CentralMoment(m_image, m_mask, p, q, centerX, centerY, m20);

   float v20 = (float)m20 / (pow(m00, (p+q)/2 + 1));

   qDebug() << v20;
#if 0
    QColor clr(255,255,255);
    for(int i = -5; i <= 5; i++)
        for(int j = -5; j <= 5; j++)
            frame.setPixelColor(centerX + i,centerY + j, clr);

#endif

#if 0
    QImage img(width, height, QImage::Format_RGB888);
    m_image->ReadImage(img.bits(), width * height * GetBytesPerPixel(PixelType::RGB24));
    m_ui->setImage(img);

#else
    if(m_mask->GetPixelType() == PixelType::Grayscale8)
    {
        QImage img(width, height, QImage::Format_Grayscale8);
        m_mask->ReadImage(img.bits(), width * height * GetBytesPerPixel(PixelType::Grayscale8));
        m_ui->setImage(img);
    }
    m_ui->setTmpImage(frame);

#endif
    m_block_ = false;
    //m_ui->setImage(frame);
}

void GestureRecognition::setUI(WebCam *ui)
{
    m_ui = ui;
}
