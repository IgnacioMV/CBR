#include "downsamplingworker.h"
#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QThread>

DownsamplingWorker::DownsamplingWorker()
{

}

void DownsamplingWorker::start()
{
    qInfo() << "downsamplingworker";
    QImage qimage;
    QPixmap *pixmap;
    QPixmap smallPixmap;
    QPixmap pixmap1;
    QPixmap pixmap2;
    if (getSourceImage2() == nullptr || getSourceImage1()->getWidth() > getSourceImage1()->getHeight()) {
        qimage = getSourceImage1()->getQImage();
        if (getW() == 0 && getH() == 0)
            smallPixmap =  QPixmap::fromImage(getSourceImage1()->getQImage());
        else if (getW() == 0)
            smallPixmap = QPixmap::fromImage(getSourceImage1()->getQImage()).scaledToHeight(getH(), Qt::FastTransformation);
        else if (getH() == 0)
            smallPixmap = QPixmap::fromImage(getSourceImage1()->getQImage()).scaledToWidth(getW(), Qt::FastTransformation);
        else
            smallPixmap= QPixmap::fromImage(getSourceImage1()->getQImage()).scaled(getW(),getH(),Qt::KeepAspectRatio, Qt::FastTransformation);
    }
    else {
        int totalWidth, totalHeight;
        int w1, w2, h1, h2;
        w1 = getSourceImage1()->getWidth();
        w2 = getSourceImage2()->getWidth();
        h1 = getSourceImage1()->getHeight();
        h2 = getSourceImage2()->getHeight();
        totalWidth = (w1 < w2) ? w1 : w2;
        totalHeight = (h1 < h2) ? h1 : h2;
        pixmap = new QPixmap(2*totalWidth+3, totalHeight);
        pixmap->fill(Qt::transparent);
        QPainter *painter = new QPainter(pixmap);
        painter->drawPixmap(0, 0, totalWidth, totalHeight, QPixmap::fromImage(getSourceImage1()->getQImage())
                            .scaled(totalWidth, totalHeight));
        QPixmap pageSep(3, pixmap->height());
        pageSep.fill(QColor(192, 192, 192, 127));
        painter->drawPixmap(getSourceImage1()->getWidth()+1, 0, 3, pixmap->height(), pageSep);
        painter->drawPixmap(totalWidth+4, 0, totalWidth, totalHeight, QPixmap::fromImage(getSourceImage2()->getQImage())
                            .scaled(totalWidth, totalHeight));
        painter->end();
        qimage = pixmap->toImage();
        if (getW() == 0 && getH() == 0)
            smallPixmap =  *pixmap;
        else if (getW() == 0)
            smallPixmap = (pixmap->scaledToHeight(getH(), Qt::FastTransformation));
        else if (getH() == 0)
            smallPixmap = (pixmap->scaledToWidth(getW(), Qt::FastTransformation));
        else
            smallPixmap= (pixmap->scaled(getW(),getH(),Qt::KeepAspectRatio, Qt::FastTransformation));
    }
    emit pixmapReady(qimage, smallPixmap, getI());
    emit finished();
}

void DownsamplingWorker::startZoom()
{
    qInfo() << "startZoom";
    emit zoomReady(QPixmap::fromImage(getSourceImage1()->getQImage()).scaled(getW(), getH(), Qt::KeepAspectRatio, Qt::FastTransformation));
    emit finished();
}
