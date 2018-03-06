#ifndef COMIC_H
#define COMIC_H

#include <QString>
#include <QList>
#include <image.h>

class Comic
{
public:
    Comic() = default;
    Comic(const Comic& comic) = default;
    explicit Comic(QString filename) : filename(filename), pageCount(-1), currentPage(0) {}
    QString getFilename() { return this->filename; }
    QList<Image*> getPages() { return this->pages; }
    Image* getPageInPosition(int position) { return this->pages.value(position);}
    void setPages(QList<Image*> pages) { this->pages = pages; }
    int getPageCount() { return this->pageCount; }
    int getCurrentPage() { return this->currentPage; }
    void setCurrentPage(int currentPage) { this->currentPage = currentPage; }
    void setPageCount(int pageCount) { this->pageCount = pageCount; }
    void sortPages() { qSort(this->pages.begin(), this->pages.end(), compareImages); }
    virtual int extract() = 0;
    virtual ~Comic(){
        for (auto img : this->pages)
            delete img;
    }

private:
    static bool compareImages(Image* i1, Image* i2) {
        int x = QString::compare(i1->getFilename(), i2->getFilename(), Qt::CaseInsensitive);
        if (x < 0)
            return true;
        else
            return false;
    }
    QString filename;
    QList<Image*> pages;
    int pageCount;
    int currentPage;
};

#endif // COMIC_H
