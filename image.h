#ifndef IMAGE_H
#define IMAGE_H

#include <QPixmap>
#include <QMimeType>
#include <QMainWindow>
#include "scalingalgorithms.h"
#include "displaymode.h"

class Image : public QObject
{
    Q_OBJECT
public:
    Image() = default;
    Image(const Image& image) = default;
    static Image* make_image(QMimeType mimeType);
    void setFilename(QString filename) { this->filename = filename; }
    void setQImage(QImage qimage) { this->qimage = qimage; }
    void setWidth(int w) { this->width = w; }
    void setHeight(int h) { this->height = h; }
    void setDoublePage(bool doublePage) { this->doublePage = doublePage; }
    void setLeftPage(bool leftPage) { this->leftPage = leftPage; }
    QString getFilename() {return this->filename; }
    QImage getQImage() { return this->qimage; }
    int getWidth() { return this->width; }
    int getHeight() { return this->height; }
    bool isDoublePage() { return this->doublePage; }
    bool isLeftPage() { return this->leftPage; }
    virtual QPixmap getPixmapForSize(int w, int h) = 0;
    virtual void getPixmapForSizeAndAlgorithm(const QMainWindow *mainWindow, int w, int h, int i,  ScalingAlgorithms algorithm, DisplayMode dispayMode) = 0;
    virtual QPixmap getThumbnail() = 0;
    virtual ~Image(){}

private:
    QString filename;
    QImage qimage;
    int width, height;
    bool doublePage;
    bool leftPage;
};

#endif // IMAGE_H
