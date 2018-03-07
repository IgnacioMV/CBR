#ifndef THUMBNAILWORKER_H
#define THUMBNAILWORKER_H

#include <QObject>
#include "image.h"

Q_DECLARE_METATYPE(QPixmap)
class ThumbnailWorker : public QObject
{
    Q_OBJECT
public:
    ThumbnailWorker(QObject *parent=0);
    ~ThumbnailWorker(){}

public slots:
    void setW(const int w);
    void setH(const int h);
    void setI(const int i);
    void setSourceImage(Image *sourceImage);
    void start();
signals:
    void error();
    void finished(const QPixmap &output);
private:
    int w, h, i;
    Image *sourceImage;

};

#endif // THUMBNAILWORKER_H
