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
    //qsrand(i);
    //int rand = qrand() % ((25 + 1) - 25*i) + 25*i;
    //qInfo() << "sleep: " << rand;
    //QThread::msleep(rand);

    QPixmap thumbnail = sourceImage->getThumbnail();
    emit thumbnailCreated( thumbnail, this->i );
    emit finished();
}
