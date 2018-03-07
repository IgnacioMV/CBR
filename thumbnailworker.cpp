#include "thumbnailworker.h"
#include <QBuffer>
#include <QImageReader>


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
    QByteArray ba = sourceImage->getBA();
    QBuffer qbuff(&ba);
    QImageReader qimg;
    qimg.setDecideFormatFromContent(true);
    qimg.setDevice(&qbuff);
    qInfo("can read: %s", (qimg.canRead()) ? "yes" : "no");
    QImage thumbnailImage = qimg.read();
    QPixmap output = QPixmap::fromImage(thumbnailImage).scaled(200,200,Qt::KeepAspectRatio, Qt::FastTransformation);
    emit finished( output );
}
