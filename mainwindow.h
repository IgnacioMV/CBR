#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QListWidget>
#include <QThread>
#include <QPixmap>
#include <QList>
#include <QThread>

#include <archive.h>
#include <archive_entry.h>

#include "comic.h"
#include "cbzcomic.h"
#include "image.h"
#include "thumbnailworker.h"
#include "displaymode.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    DisplayMode getDisplayMode() { return this->displayMode; }
    ~MainWindow();

public slots:
    void pageReady();
    //void previousPageReady();
    //void currentPageReady();
    //void nextPageReady();
    void pixmapReady(const QImage &qimage, const QPixmap &pixmap, const int i);
    void zoomReady(const QPixmap &pixmap);
    void finishedExtraction();

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

    void fitWidth();
    void fitHeight();
    void fitToWindow();
    void fitOriginal();

    void downsamplingMode();
    void bilinearMode();
    void bicubicMode();

    void setThumbnail(const QPixmap &thumbnail, const int i);
    void onThumbnailDoubleClick(QListWidgetItem *item);

    void nextPage();
    void previousPage();
    void firstPage();
    void lastPage();

private:
    Comic *comic;
    QList<QThread*> thumbnailThreads;

    void scaleImage(double factor);
    //void resizeEvent(QResizeEvent* resizeEvent);
    int displayImageInPosition(int position);
    int displayTwoImageInPosition(int position);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void asyncResizeImageForAlgorithm(int i, int width, int height, ScalingAlgorithms algorithm);
    void asyncZoomForAlgorithm(int width, int height, ScalingAlgorithms algorithm);
    void processNextPixmap();
    void processPreviousPixmap();
    void setViewModeIcon(QAction *act);
    void changeSelectedDisplayMode(DisplayMode mode);
    void changeSelectedAlgorithm(ScalingAlgorithms algorithm);

    bool comicOpened;
    double scaleFactor;
    bool twoPage;
    bool firstPageShown;

    QImage previousImage;
    QImage currentImage;
    QImage nextImage;
    QImage firstImage;
    QImage lastImage;

    QPixmap zoomPixmap;
    QPixmap previousPixmap;
    QPixmap currentPixmap;
    QPixmap nextPixmap;
    QPixmap lastPixmap;
    QPixmap firstPixmap;

    Ui::MainWindow *ui;
    QListWidget *thumbnailList;
    QLabel *imageLabel;
    QScrollArea *scrollArea;
    QPushButton *firstPageButton;
    QPushButton *previousPageButton;
    QPushButton *nextPageButton;
    QPushButton *lastPageButton;
    QLabel *currentPageLabel;

    int hb0, vb0, x0, y0, x, y;
    DisplayMode displayMode;
    ScalingAlgorithms scalingAlgorithm;

    QAction *twoPagesAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;

    QAction *fitWidthAct;
    QAction *fitHeightAct;
    QAction *fitToWindowAct;
    QAction *originalAct;

    QAction *downsamplingAct;
    QAction *bilinearAct;
    QAction *bicubicAct;

    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // MAINWINDOW_H
