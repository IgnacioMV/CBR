#include "cbzcomic.h"
#include "comic.h"
#include "image.h"
#include "bmp.h"
#include "jpeg.h"
#include "png.h"
#include "cbzextractworker.h"
#include <QMainWindow>
#include <QDebug>
#include <QThread>

int CBZComic::extract(const QMainWindow *mainWindow)
{
    qInfo() << "thumbnailThread->isRunning()";
    QThread* thread= new QThread();
    cbzWorker = new CBZExtractWorker();
    cbzWorker->moveToThread( thread );

    QObject::connect( thread, SIGNAL(started()), cbzWorker, SLOT(start()) );
    QObject::connect( cbzWorker, SIGNAL(pageExtracted(Image *)), this, SLOT(addPage(Image *)));
    QObject::connect( cbzWorker, SIGNAL(pageReady()), mainWindow, SLOT(pageReady()));
    QObject::connect( cbzWorker, SIGNAL(finished()), this, SLOT(finishedExtraction()));
    QObject::connect( cbzWorker, SIGNAL(finished()), mainWindow, SLOT(finishedExtraction()));
    QObject::connect( cbzWorker, SIGNAL(finished()), thread, SLOT(quit()));

    cbzWorker->setFilename(getFilename());
    thumbnailThread.clear();
    thumbnailThread.append(thread);
    thread->start();

    return 0;
}

void CBZComic::stopExtracting()
{
    qInfo() << "begin stop";
    qInfo() << getThumbnailThread().size();
    cbzWorker->setRun(false);
    if (!thumbnailThread.empty()) {
        thumbnailThread.value(0)->exit();
        thumbnailThread.value(0)->wait();
    }
    qInfo() << "finish stop";
}
