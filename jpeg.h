#ifndef JPEG_H
#define JPEG_H

#include "image.h"
#include <QMainWindow>

class JPEG: public Image
{
    Q_OBJECT
public:
    JPEG();
    QPixmap getPixmapForSize(int w, int h);
    void getPixmapForSizeAndAlgorithm(const QMainWindow *mainWindow, int w, int h, int i, ScalingAlgorithms algorithm);
    QPixmap getThumbnail();
    ~JPEG(){}
};

#endif // JPEG_H
