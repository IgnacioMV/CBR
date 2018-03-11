#ifndef PNG_H
#define PNG_H

#include "image.h"
#include <QMainWindow>
#include "displaymode.h"

class PNG: public Image
{
    Q_OBJECT
public:
    PNG();
    QPixmap getPixmapForSize(int w, int h);
    void getPixmapForSizeAndAlgorithm(const QMainWindow *mainWindow, int w, int h, int i, ScalingAlgorithms algorithm, DisplayMode dispayMode);
    QPixmap getThumbnail();
    ~PNG(){}
};

#endif // PNG_H
