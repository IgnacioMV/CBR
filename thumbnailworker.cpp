#include "thumbnailworker.h"
#include <QBuffer>
#include <QImageReader>
#include <QThread>
#include <QDebug>


ThumbnailWorker::ThumbnailWorker(QObject *parent)
{

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
    qsrand(i);
    int rand = qrand() % ((50 + 1) - 50*i) + 50*i;
    qInfo() << "sleep: " << rand;
    QThread::msleep(rand);
    QByteArray ba = sourceImage->getBA();
    QBuffer qbuff(&ba);
    QImageReader qimg;
    qimg.setDecideFormatFromContent(true);
    qimg.setDevice(&qbuff);
    QImage thumbnailImage = qimg.read();
    thumbnail = QPixmap::fromImage(thumbnailImage).scaled(200,200,Qt::KeepAspectRatio, Qt::FastTransformation);
    emit finished( thumbnail, this->i );
}
