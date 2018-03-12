#ifndef EXTRACTWORKER_H
#define EXTRACTWORKER_H

#include <QObject>
#include "image.h"

class ExtractWorker : public QObject
{
    Q_OBJECT
public:
    ExtractWorker(){ this->run = true; }
    ~ExtractWorker(){}
    void setRun(bool run) { this->run = run; }
    void setFilename(const QString filename) { this->filename = filename; }
    bool getRun() { return this->run; }
    QString getFilename() { return this->filename; }
public slots:
    virtual void start() = 0;
signals:
    void error();
    void pageExtracted(Image *img);
    void pageReady();
    void finished();
private:
    QString filename;
    bool run;
};

#endif // EXTRACTWORKER_H
