#ifndef CBZCOMIC_H
#define CBZCOMIC_H

#include <archive.h>
#include <archive_entry.h>
#include <QByteArray>
#include <QMimeDatabase>
#include <QMimeType>
#include <image.h>
#include <QDebug>
#include <comic.h>


class CBZComic: public Comic {
public:
    CBZComic() : Comic() {}
    CBZComic(const CBZComic &cbzcomic) = default;
    explicit CBZComic(QString filename) : Comic(filename) {}
    int extract();
    ~CBZComic(){}
};

#endif // CBZCOMIC_H
