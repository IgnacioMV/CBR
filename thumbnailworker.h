#ifndef THUMBNAILWORKER_H
#define THUMBNAILWORKER_H

#include <QObject>
#include "image.h"

class ThumbnailWorker : public QObject
{
    Q_OBJECT
public:
    ThumbnailWorker(QObject *parent=0);
    ~ThumbnailWorker(){}
    QPixmap getThumbnail() { return this->thumbnail; }

public slots:
    void setW(const int w);
    void setH(const int h);
    void setI(const int i);
    void setSourceImage(Image *sourceImage);
    void start();
signals:
    void error();
    void finished(const QPixmap &output, const int &i);
private:
    int w, h, i;
    Image *sourceImage;
    QPixmap thumbnail;

};

#endif // THUMBNAILWORKER_H
