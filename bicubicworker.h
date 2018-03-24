#ifndef BICUBICWORKER_H
#define BICUBICWORKER_H

#include "scaleimageworker.h"
//#include <QImage>

class BicubicWorker: public ScaleImageWorker
{
    Q_OBJECT
public:
    BicubicWorker();
public slots:
    void start();
    void startZoom();
private:
    QPixmap resize(QImage *src);
};

#endif // BICUBICWORKER_H
