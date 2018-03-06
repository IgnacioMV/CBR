#ifndef JPEG_H
#define JPEG_H

#include <image.h>

class JPEG: public Image
{
public:
    JPEG();
    QPixmap getPixmapForResolution(int w, int h);
    QPixmap getThumbnail();
    ~JPEG(){}
};

#endif // JPEG_H
