#ifndef CBZCOMIC_H
#define CBZCOMIC_H

#include <archive.h>
#include <archive_entry.h>
#include <QByteArray>
#include <QMimeDatabase>
#include <QMimeType>
#include "image.h"
#include <QDebug>
#include "comic.h"
#include <QMainWindow>
#include "cbzextractworker.h"
#include <QObject>

class CBZComic: public Comic
{
    Q_OBJECT
private slots:

    void finishedExtraction(){}

public:
    CBZComic() : Comic() {}
    CBZComic(const CBZComic &cbzcomic) = default;
    explicit CBZComic(QString filename) : Comic(filename) {}
    int extract(const QMainWindow *mainWindow);
    void stopExtracting();
    ~CBZComic(){ delete cbzWorker; }

private:
    CBZExtractWorker *cbzWorker;
};

#endif // CBZCOMIC_H
