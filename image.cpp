#include "image.h"
#include "bmp.h"
#include "jpeg.h"
#include "png.h"
#include <QDebug>

Image *Image::make_image(QMimeType mimeType) {
    QString mimeName = mimeType.name();
    if (mimeName.contains("image/png", Qt::CaseInsensitive))
        return new PNG;
    else if(mimeName.contains("image/jpeg", Qt::CaseInsensitive))
        return new JPEG;
    else if(mimeName.contains("image/bmp", Qt::CaseInsensitive))
        return new BMP;
    else {
        return Q_NULLPTR;
    }
}
