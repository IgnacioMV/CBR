#ifndef CBZEXTRACTWORKER_H
#define CBZEXTRACTWORKER_H

#include "extractworker.h"
#include "image.h"

class CBZExtractWorker : public ExtractWorker
{
    Q_OBJECT
public:
    CBZExtractWorker(): ExtractWorker(){}
    ~CBZExtractWorker(){}
public slots:
    void start();/*
signals:
    void error();
    void pageExtracted(Image *img);
    void pageReady();
    void finished();
private:
    QString filename;
    bool run;*/
};

#endif // CBZEXTRACTWORKER_H
