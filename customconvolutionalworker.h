#ifndef CUSTOMCONVOLUTIONALWORKER_H
#define CUSTOMCONVOLUTIONALWORKER_H

#include "scaleimageworker.h"

class CustomConvolutionalWorker: public ScaleImageWorker
{
    Q_OBJECT
public:
    CustomConvolutionalWorker();
public slots:
    void start();
    void startZoom();
};

#endif // CUSTOMCONVOLUTIONALWORKER_H
