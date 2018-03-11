#include "png.h"
#include <QByteArray>
#include <QBuffer>
#include <QImageReader>
#include <QImage>
#include <QPixmap>
#include <QMainWindow>
#include "scaleimageworker.h"

PNG::PNG() : Image()
{

}

QPixmap PNG::getThumbnail()
{
    return PNG::getPixmapForSize(150, 150);
}

QPixmap PNG::getPixmapForSize(int w, int h)
{
    return QPixmap::fromImage(this->getQImage()).scaled(w,h,Qt::KeepAspectRatio, Qt::FastTransformation);
}

void PNG::getPixmapForSizeAndAlgorithm(const QMainWindow *mainWindow, int w, int h, int i, ScalingAlgorithms algorithm, DisplayMode dispayMode)
{

}
