#ifndef IMAGE_H
#define IMAGE_H

class Image
{
public:
    void setFilename(QString filename) { this->filename = filename; }
    void setBA(QByteArray ba) { this->ba = ba; };
    QString getFilename() const {return this->filename; };
    QByteArray getBA() const { return this->ba; };


private:


    QString filename;
    QByteArray ba;

};

#endif // IMAGE_H
