#ifndef PNG_H
#define PNG_H

#include <image.h>

class PNG: public Image
{
    Q_OBJECT
public:
    PNG();
    QPixmap getPixmapForResolution(int w, int h);
    QPixmap getThumbnail();
    ~PNG(){}
};

#endif // PNG_H
