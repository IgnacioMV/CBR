#ifndef IMAGE_H
#define IMAGE_H

#include <QPixmap>

class Image
{
public:
    Image() = default;
    Image(const Image& image) = default;
    void setFilename(QString filename) { this->filename = filename; }
    void setBA(QByteArray ba) { this->ba = ba; };
    QString getFilename() {return this->filename; };
    QByteArray getBA() { return this->ba; };
    virtual QPixmap getPixmapForResolution(int w, int h) = 0;
    virtual ~Image(){};

private:
    QString filename;
    QByteArray ba;
};

#endif // IMAGE_H