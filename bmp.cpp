#include "bmp.h"
#include <QByteArray>
#include <QBuffer>
#include <QImageReader>
#include <QImage>
#include <QPixmap>
#include <QMainWindow>
#include "scaleimageworker.h"

BMP::BMP() : Image()
{

}

QPixmap BMP::getThumbnail()
{
    return BMP::getPixmapForSize(150, 150);
}

QPixmap BMP::getPixmapForSize(int w, int h)
{
    return QPixmap::fromImage(this->getQImage()).scaled(w,h,Qt::KeepAspectRatio, Qt::FastTransformation);
}

void BMP::getPixmapForSizeAndAlgorithm(const QMainWindow *mainWindow, int w, int h, int i, ScalingAlgorithms algorithm, DisplayMode dispayMode)
{

}
