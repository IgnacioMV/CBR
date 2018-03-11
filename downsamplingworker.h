#ifndef DOWNSAMPLINGWORKER_H
#define DOWNSAMPLINGWORKER_H

#include "scaleimageworker.h"

class DownsamplingWorker: public ScaleImageWorker
{
    Q_OBJECT
public:
    DownsamplingWorker();
public slots:
    void start();
    void startZoom();
};

#endif // DOWNSAMPLINGWORKER_H
