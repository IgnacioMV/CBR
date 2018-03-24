#include "bicubicworker.h"
#include <QBuffer>
#include <QDebug>
#include <QPainter>
#include <QImageReader>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

BicubicWorker::BicubicWorker()
{

}

void BicubicWorker::start()
{
    qInfo() << "bicubicworker";
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
            smallPixmap = resize(&qimage);
        else if (getH() == 0)
            smallPixmap = resize(&qimage);
        else
            smallPixmap= resize(&qimage);
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
            smallPixmap = resize(&qimage);
        else if (getH() == 0)
            smallPixmap = resize(&qimage);
        else
            smallPixmap= resize(&qimage);
    }
    emit pixmapReady(qimage, smallPixmap, getI());
    emit finished();
}

void BicubicWorker::startZoom()
{
    qInfo() << "startZoom";
    QImage img = getSourceImage1()->getQImage();
    emit zoomReady(resize(&img));
    emit finished();
}

QPixmap BicubicWorker::resize(QImage *src)
{
    const char* format = "BMP";
    QString extension = ".bmp";

    double scaleW, scaleH, scale;
    scaleW = (double) getW()/ src->width();
    scaleH = (double) getH()/(src->height());
    qInfo() << "getW : " << getW();
    qInfo() << "width : " << src->width();
    qInfo() << "scaleH : " << scaleH;
    if (getW() == 0)
        scale = scaleH;
    else if (getH() == 0)
        scale = scaleW;
    else
        scale = (scaleH < scaleW) ? scaleH : scaleW;
    qInfo() << "scale : " << scale;
    QByteArray arr;
    QBuffer buffer(&arr);
    buffer.open(QIODevice::WriteOnly);
    src->save(&buffer, format);
    std::string data(arr.constData(), arr.length());
    std::vector<uchar> vectordata(data.begin(),data.end());

    cv::Mat data_mat = cv::imdecode(vectordata, true);

    cv::Mat dst;
    cv::resize(data_mat, dst, cv::Size(data_mat.cols * scale, data_mat.rows * scale), 0, 0, cv::INTER_LINEAR);

    std::vector<uchar> outData;
    cv::imencode(extension.toStdString(), dst, outData);

    QByteArray qba = QByteArray::fromRawData(reinterpret_cast<const char*>(outData.data()), outData.size());
    QBuffer qbuff(&qba);
    QImageReader reader(&qbuff);
    QImage out = reader.read();

    return QPixmap::fromImage(out);
}
