#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QListWidget>

#include <archive.h>
#include <archive_entry.h>

#include <comic.h>
#include <cbzcomic.h>
#include <image.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

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

    double scaleFactor;
    bool twoPage;
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
