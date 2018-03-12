#include "scaleimageworker.h"
#include "bilinearworker.h"
#include "downsamplingworker.h"

ScaleImageWorker::ScaleImageWorker()
{

}

ScaleImageWorker::~ScaleImageWorker()
{

}

//Factory
ScaleImageWorker *ScaleImageWorker::make_scaleImageWorker(const ScalingAlgorithms algorithm)
{
    if (algorithm ==  ScalingAlgorithms::Downsampling)
        return new DownsamplingWorker;
    else if (algorithm ==  ScalingAlgorithms::Bilinear)
        return new BilinearWorker;
    else
        return new DownsamplingWorker;
}

void ScaleImageWorker::setSourceImages(Image *sourceImage1, Image *sourceImage2)
{
    this->sourceImage1 = sourceImage1;
    this->sourceImage2 = sourceImage2;
}

void ScaleImageWorker::setW(int w)
{
    this->w = w;
}

void ScaleImageWorker::setH(int h)
{
    this->h = h;
}

void ScaleImageWorker::setI(int i)
{
    this->i = i;
}

