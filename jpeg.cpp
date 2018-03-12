#include "jpeg.h"
#include <QDebug>
#include <QByteArray>
#include <QBuffer>
#include <QImageReader>
#include <QImage>
#include <QPixmap>
#include <QMainWindow>
#include <QThread>
#include "scaleimageworker.h"
#include <QMainWindow>

JPEG::JPEG() : Image()
{

}

QPixmap JPEG::getThumbnail()
{
    return JPEG::getPixmapForSize(150, 150);
}

QPixmap JPEG::getPixmapForSize(int w, int h)
{
    return QPixmap::fromImage(this->getQImage()).scaled(w,h,Qt::KeepAspectRatio, Qt::FastTransformation);
}

void JPEG::getPixmapForSizeAndAlgorithm(const QMainWindow *mainWindow, int w, int h,  int i, ScalingAlgorithms algorithm, DisplayMode displayMode)
{
    QThread *thread = new QThread();
    ScaleImageWorker *siWorker = ScaleImageWorker::make_scaleImageWorker(algorithm);

    siWorker->moveToThread( thread );
    QObject::connect( thread, SIGNAL(started()), siWorker, SLOT(start()) );
    QObject::connect( siWorker, SIGNAL(pixmapReady(const QPixmap &, const int &)), mainWindow, SLOT(pixmapReady(const QPixmap &, const int &)));
    QObject::connect( siWorker, SIGNAL(finished()), thread, SLOT(quit()));

    siWorker->setW(w);
    siWorker->setH(h);
    siWorker->setI(i);
    siWorker->setDisplayMode(displayMode);
    siWorker->setSourceImages(this, nullptr);
    thread->start();
}
