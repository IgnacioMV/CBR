#ifndef BMP_H
#define BMP_H

#include <image.h>

class BMP: public Image
{
public:
    BMP();
    QPixmap getPixmapForResolution(int w, int h);
    QPixmap getThumbnail();
    ~BMP(){}
};

#endif // BMP_H
