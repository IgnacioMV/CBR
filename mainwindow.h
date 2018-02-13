#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QScrollArea>

#include <archive.h>
#include <archive_entry.h>

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
    void openFile();
    void closeFile();
    void zoomIn();
    void zoomOut();
    void normalSize();

    void nextPage();
    void previousPage();
    void firstPage();
    void lastPage();

private:
    void scaleImage(double factor);

    //void resizeEvent(QResizeEvent* resizeEvent);

    int displayImageInPosition(int position);
    static bool compareImages(const Image &i1, const Image &i2);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    double scaleFactor;
    Ui::MainWindow *ui;
    QString currentFile;
    QLabel *imageLabel;
    QScrollArea *scrollArea;
    QImage currentImage;

    struct archive *arch;
    QList<Image> imageList;
    int maxPage;
    int currentPage;
    int hb0, vb0, x0, y0, x, y;

    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct ;

    bool eventFilter(QObject *obj, QEvent *event);
};

#endif // MAINWINDOW_H
