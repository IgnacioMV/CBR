#include "thumbnailworker.h"
#include <QBuffer>
#include <QImageReader>
#include <QThread>
#include <QDebug>

ThumbnailWorker::ThumbnailWorker()
{

}

ThumbnailWorker::~ThumbnailWorker()
{
    delete &thumbnail;
}

void ThumbnailWorker::setW(const int w)
{
    this->w = w;
}
void ThumbnailWorker::setH(const int h)
{
    this->h = h;
}
void ThumbnailWorker::setI(const int i)
{
    this->i = i;
}
void ThumbnailWorker::setSourceImage(Image *sourceImage)
{
    this->sourceImage = sourceImage;
}
void ThumbnailWorker::start()
{
    if (sourceImage == NULL)
        return;

    QPixmap thumbnail = sourceImage->getThumbnail();
    emit thumbnailCreated( thumbnail, this->i );
    emit finished();
}
