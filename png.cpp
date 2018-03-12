#include "png.h"
#include <QByteArray>
#include <QBuffer>
#include <QImageReader>
#include <QImage>
#include <QPixmap>
#include <QThread>
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

void PNG::getPixmapForSizeAndAlgorithm(const QMainWindow *mainWindow, int w, int h, int i, ScalingAlgorithms algorithm)
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
    siWorker->setSourceImages(this, nullptr);
    thread->start();
}
