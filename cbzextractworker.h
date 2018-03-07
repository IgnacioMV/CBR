#ifndef CBZEXTRACTWORKER_H
#define CBZEXTRACTWORKER_H

#include <QObject>
#include <image.h>

class CBZExtractWorker : public QObject
{
    Q_OBJECT
public:
    CBZExtractWorker();
    ~CBZExtractWorker(){}
public slots:
    void setFilename(const QString filename);
    void start();
signals:
    void error();
    void pageExtracted(Image *img);
    void pageReady();
    void finished();
private:
    QString filename;
};

#endif // CBZEXTRACTWORKER_H
