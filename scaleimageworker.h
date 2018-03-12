#ifndef SCALEIMAGEWORKER_H
#define SCALEIMAGEWORKER_H

#include <QObject>
#include "image.h"
#include "displaymode.h"

class ScaleImageWorker : public QObject
{
    Q_OBJECT
public:
    ScaleImageWorker();

    //enum ScalingAlgorithm { Downsampling, Bilinear, Bicubic, CustomConvolutional };
    //Q_ENUM(ScalingAlgorithm)

    //Factory
    static ScaleImageWorker *make_scaleImageWorker(ScalingAlgorithms algorithm);
    void setSourceImages(Image* sourceImage1, Image* sourceImage2);
    void setW(int w);
    void setH(int h);
    void setI(int i);
    Image* getSourceImage1() { return this->sourceImage1; }
    Image* getSourceImage2() { return this->sourceImage2; }
    int getW() { return w; }
    int getH() { return h; }
    int getI() { return i; }
    ~ScaleImageWorker();

public slots:

    virtual void start() = 0;
    virtual void startZoom() = 0;

signals:
    void error();
    void pixmapReady(const QImage &qimage, const QPixmap &output, const int &i);
    void zoomReady(const QPixmap &pixmap);
    void finished();

private:
    int w, h, i;
    Image *sourceImage1;
    Image *sourceImage2;
};

#endif // SCALEIMAGEWORKER_H
