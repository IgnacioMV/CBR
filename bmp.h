#ifndef BMP_H
#define BMP_H

#include "image.h"
#include <QMainWindow>

class BMP: public Image
{
    Q_OBJECT
public:
    BMP();
    QPixmap getPixmapForSize(int w, int h);
    void getPixmapForSizeAndAlgorithm(const QMainWindow *mainWindow, int w, int h, int i, ScalingAlgorithms algorithm);
    QPixmap getThumbnail();
    ~BMP(){}
};

#endif // BMP_H
