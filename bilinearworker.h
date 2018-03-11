#ifndef BILINEARWORKER_H
#define BILINEARWORKER_H

#include "scaleimageworker.h"

class BilinearWorker: public ScaleImageWorker
{
    Q_OBJECT
public:
    BilinearWorker();
public slots:
    void start();
    void startZoom();
};

#endif // BILINEARWORKER_H
