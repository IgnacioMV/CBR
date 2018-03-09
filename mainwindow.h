#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QListWidget>
#include <QThread>
#include <QPixmap>

#include <archive.h>
#include <archive_entry.h>

#include <comic.h>
#include <cbzcomic.h>
#include <image.h>
#include <thumbnailworker.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void pageReady() {
        updatePageActions();
        if (firstPageShown)
            return;
        qInfo() << "first page size: " << comic->getPageCount();
        (twoPage) ? displayTwoImageInPosition(0) : displayImageInPosition(0);
        updatePageActions();
        firstPageShown = true;
    }
    void finishedExtraction() {
        QPixmap emptyThumbnail(200, 200);
        emptyThumbnail.fill("grey");

        for (int i = 0; i < comic->getPageCount(); i++) {
            QListWidgetItem *itm = new QListWidgetItem(QString::number(i));
            itm->setTextAlignment(Qt::AlignCenter);
            itm->setSizeHint(QSize(200, 200));
            itm->setIcon(emptyThumbnail);
            //itm->setIcon(QIcon(QPixmap::fromImage(currentImage).scaled(200,200,Qt::KeepAspectRatio, Qt::FastTransformation)));
            thumbnailList->setIconSize(QSize(150,150));
            thumbnailList->addItem(itm);

            QThread *thread = new QThread();
            ThumbnailWorker *tWorker = new ThumbnailWorker();
            tWorker->moveToThread( thread );
            QObject::connect( thread, SIGNAL(started()), tWorker, SLOT(start()) );
            QObject::connect( tWorker, SIGNAL(thumbnailCreated(const QPixmap &, const int &)), this, SLOT(setThumbnail(const QPixmap &, const int &)));
            QObject::connect( tWorker, SIGNAL(finished()), thread, SLOT(&QThread::quit));

            tWorker->setW(150);
            tWorker->setH(150);
            tWorker->setI(i);
            tWorker->setSourceImage(comic->getPages().value(i));

            thread->start();
        }
    }

private slots:
    void createActions();
    void enablePageActions();
    void disablePageActions();
    void updatePageActions();
    void openFile();
    void closeFile();
    void twoPages();
    void zoomIn();
    void zoomOut();
    void normalSize();

    void setThumbnail(const QPixmap &thumbnail, const int i);
    void onThumbnailDoubleClick(QListWidgetItem *item);

    void nextPage();
    void previousPage();
    void firstPage();
    void lastPage();

private:
    Comic *comic;

    void scaleImage(double factor);
    //void resizeEvent(QResizeEvent* resizeEvent);
    int displayImageInPosition(int position);
    int displayTwoImageInPosition(int position);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    enum DisplayMode { FitToWindow, FitToWidth, FitToHeight, Original };
    Q_ENUM(DisplayMode)
    double scaleFactor;
    bool twoPage;
    bool firstPageShown;
    QPixmap nextPixmap;
    QPixmap previousPixmap;
    QPixmap lastPixmap;
    QPixmap firstPixmap;

    Ui::MainWindow *ui;
    QListWidget *thumbnailList;
    QLabel *imageLabel;
    QScrollArea *scrollArea;
    QImage currentImage;
    QPushButton *firstPageButton;
    QPushButton *previousPageButton;
    QPushButton *nextPageButton;
    QPushButton *lastPageButton;
    QLabel *currentPageLabel;

    int hb0, vb0, x0, y0, x, y;

    QAction *twoPagesAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;

    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // MAINWINDOW_H
