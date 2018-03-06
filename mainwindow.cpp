#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QListWidget>
#include <QFileDialog>
#include <QImageReader>
#include <QBuffer>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMimeDatabase>
#include <QMimeType>
#include <QtDebug>
#include <QScrollBar>
#include <QMouseEvent>
#include <QPainter>

#include <comic.h>
#include <cbzcomic.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {

    QWidget *centralWidget = new QWidget;
    this->setCentralWidget(centralWidget);
    QVBoxLayout *centralLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout *midLayout = new QHBoxLayout;

    thumbnailList = new QListWidget;
    thumbnailList->setViewMode(QListView::IconMode);
    thumbnailList->setMovement(QListView::Static);
    thumbnailList->setFixedWidth(200);
    //new QListWidgetItem(tr("Oak"), thumbnailList);
    //new QListWidgetItem(tr("Fir"), thumbnailList);
    //new QListWidgetItem(tr("Pine"), thumbnailList);

    scaleFactor = 1.0;
    twoPage = false;
    scrollArea = new QScrollArea();
    imageLabel = new QLabel();
    //QPixmap pixmap("/home/nacho/CBR/example.png");
    //imageLabel->setPixmap(pixmap);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true); //It scales the image of the label so that the image fits the label, not bigger or smaller.
    scrollArea->setWidgetResizable(false);//This property holds whether the scroll area should resize the view widget
    scrollArea->setWidget(imageLabel);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->installEventFilter(this);
    scrollArea->horizontalScrollBar()->installEventFilter(this);
    scrollArea->verticalScrollBar()->installEventFilter(this);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    firstPageButton = new QPushButton("First");
    previousPageButton = new QPushButton("Previous");
    nextPageButton = new QPushButton("Next");
    lastPageButton = new QPushButton("Last");

    currentPageLabel = new QLabel(" ");
    currentPageLabel->setStyleSheet("QLabel { background-color : white }");
    currentPageLabel->setFixedWidth(50);
    currentPageLabel->setAlignment(Qt::AlignCenter);

    //QIcon icon = style()->standardIcon(QStyle::SP_ArrowForward);
    //QPixmap pixmap = icon.pixmap(QSize(64, 64));
    //nextPageButton->setIcon(icon);

    QObject::connect(nextPageButton, SIGNAL(clicked()), this, SLOT(nextPage()));
    QObject::connect(previousPageButton,SIGNAL(clicked()), this, SLOT(previousPage()));
    QObject::connect(firstPageButton,SIGNAL(clicked()), this, SLOT(firstPage()));
    QObject::connect(lastPageButton,SIGNAL(clicked()), this, SLOT(lastPage()));

    btnLayout->addWidget(firstPageButton);
    btnLayout->addWidget(previousPageButton);
    btnLayout->addWidget(currentPageLabel);
    btnLayout->addWidget(nextPageButton);
    btnLayout->addWidget(lastPageButton);

    midLayout->addWidget(thumbnailList);
    midLayout->addWidget(scrollArea);

    centralLayout->addLayout(midLayout);
    centralLayout->addLayout(btnLayout);

    createActions();
    updatePageActions();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        x0 = mouseEvent->pos().x();
        y0 = mouseEvent->pos().y();
        hb0 = scrollArea->horizontalScrollBar()->value();
        vb0 = scrollArea->verticalScrollBar()->value();
    }
    if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        scrollArea->horizontalScrollBar()->setValue(hb0+x0-mouseEvent->pos().x());
        scrollArea->verticalScrollBar()->setValue(vb0+y0-mouseEvent->pos().y());
        if (scrollArea->horizontalScrollBar()->value() == scrollArea->horizontalScrollBar()->maximum() ||
                scrollArea->horizontalScrollBar()->value() == 0){
            x0 = mouseEvent->pos().x();
            hb0 = scrollArea->horizontalScrollBar()->value();
        }
        if (scrollArea->verticalScrollBar()->value() == scrollArea->verticalScrollBar()->maximum() ||
                scrollArea->verticalScrollBar()->value() == 0) {
            y0 = mouseEvent->pos().y();
            vb0 = scrollArea->verticalScrollBar()->value();
        }
    }
    if (event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        if ((wheelEvent->modifiers() & Qt::ControlModifier) && (wheelEvent->modifiers() & Qt::ShiftModifier)) {
            if (wheelEvent->delta() > 0 && scaleFactor < 6.0)
                zoomIn();
            else if(wheelEvent->delta() < 0 && scaleFactor > 1)
                zoomOut();
            return true;
        }

    }
    return false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *openAct = fileMenu->addAction(tr("&Open..."), this, &MainWindow::openFile);
    openAct->setShortcuts(QKeySequence::Open);

    QAction *closeAct = fileMenu->addAction(tr("&Close..."), this, &MainWindow::closeFile);
    closeAct->setShortcuts(QKeySequence::Close);

    fileMenu->addSeparator();

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    twoPagesAct = viewMenu->addAction(tr("Two page mode"), this, &MainWindow::twoPages);
    twoPagesAct->setEnabled(true);
    twoPagesAct->setCheckable(true);
    twoPagesAct->setShortcut(tr("Ctrl+T"));

    zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &MainWindow::zoomIn);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);

    zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &MainWindow::zoomOut);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);

    normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &MainWindow::normalSize);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
}

void MainWindow::enablePageActions()
{
    firstPageButton->setEnabled(true);
    nextPageButton->setEnabled(true);
    previousPageButton->setEnabled(true);
    lastPageButton->setEnabled(true);
}

void MainWindow::disablePageActions()
{
    firstPageButton->setEnabled(false);
    nextPageButton->setEnabled(false);
    previousPageButton->setEnabled(false);
    lastPageButton->setEnabled(false);
}

void MainWindow::updatePageActions()
{
    if (comic == NULL) {
        disablePageActions();
        return ;
    }
    enablePageActions();
    if (comic->getCurrentPage() == comic->getPageCount()-1) {
        nextPageButton->setEnabled(false);
        lastPageButton->setEnabled(false);
    }
    else if (comic->getCurrentPage() == 0) {
        previousPageButton->setEnabled(false);
        firstPageButton->setEnabled(false);
    }
}

/*
void MainWindow::resizeEvent(QResizeEvent* event)
{
    qInfo() << "resizeEvent";
    QMainWindow::resizeEvent(event);
    int w = scrollArea->width();
    int h = scrollArea->height();
    qInfo() << "w: " << w << ", h: " << h;
    imageLabel->setPixmap(QPixmap::fromImage(currentImage).scaled(w,h,Qt::KeepAspectRatio));
}
*/
void MainWindow::openFile()
{

    QString fileName = QFileDialog::getOpenFileName(this, "Open the file", "/home/nacho/CBR_old", tr("Comic files (*.cbr *.cbz)"));
    if(fileName.isEmpty() || fileName.isNull()){
        return;
    }
    closeFile();
    comic = new CBZComic(fileName);
    comic->extract();
    qInfo() << twoPage << comic->getCurrentPage();
    (twoPage) ? displayTwoImageInPosition(comic->getCurrentPage()) : displayImageInPosition(comic->getCurrentPage());
    updatePageActions();
}

void MainWindow::closeFile()
{
    delete comic;
    comic = NULL;
    imageLabel->setPixmap(QPixmap());
    qInfo() << "file closed";
    updatePageActions();
}

void MainWindow::twoPages()
{
    twoPage = twoPagesAct->isChecked();
    if (comic != NULL)
        (twoPage) ? displayTwoImageInPosition(comic->getCurrentPage()) : displayImageInPosition(comic->getCurrentPage());
}

void MainWindow::zoomIn()
{
    scaleImage(1.25);
}

void MainWindow::zoomOut()
{
    scaleImage(0.8);
}

void MainWindow::normalSize()
{
    int w = scrollArea->width();
    int h = scrollArea->height();
    imageLabel->setPixmap(QPixmap::fromImage(currentImage).scaled(w,h,Qt::KeepAspectRatio, Qt::FastTransformation));
    imageLabel->adjustSize();
    zoomInAct->setEnabled(true);
    zoomOutAct->setEnabled(false);
    scaleFactor = 1.0;
}

void MainWindow::scaleImage(double factor)
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    //imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());
    int w = scrollArea->width();
    int h = scrollArea->height();
    qInfo() << "w: " << w;
    imageLabel->setPixmap(QPixmap::fromImage(currentImage).scaled(w*scaleFactor,h*scaleFactor,Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->adjustSize();
    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 6.0);
    zoomOutAct->setEnabled(scaleFactor > 1.0);
}

void MainWindow::nextPage()
{
    int nextPosition = comic->getCurrentPage()+((twoPage) ? 2 : 1);
    (twoPage) ? displayTwoImageInPosition(nextPosition) : displayImageInPosition(nextPosition);
    updatePageActions();
}

void MainWindow::previousPage()
{
    int nextPosition = comic->getCurrentPage()-((twoPage) ? 2 : 1);
    (twoPage) ? displayTwoImageInPosition(nextPosition) : displayImageInPosition(nextPosition);
    updatePageActions();
}

void MainWindow::firstPage()
{
    (twoPage) ? displayTwoImageInPosition(0) : displayImageInPosition(0);
    updatePageActions();
}

void MainWindow::lastPage()
{
    int lastPage = comic->getPageCount()-1;
    (twoPage) ? displayTwoImageInPosition(lastPage) : displayImageInPosition(lastPage);
    updatePageActions();
}

int MainWindow::displayImageInPosition(int position)
{
    qInfo() << "position: " << position;
    if (position < 0 || position > (comic->getPageCount()-1))
        return -1;

    Image* nextImage = comic->getPageInPosition(position);
    qInfo() << "appended filename: " << nextImage->getFilename();
    qInfo() << "appended ba size: " << nextImage->getBA().size();
    QByteArray ba = nextImage->getBA();
    QBuffer qbuff(&ba);
    QImageReader qimg;
    qimg.setDecideFormatFromContent(true);
    qimg.setDevice(&qbuff);
    qInfo("can read: %s", (qimg.canRead()) ? "yes" : "no");
    currentImage = qimg.read();
    comic->setCurrentPage(position);
    normalSize();
    currentPageLabel->setText(QString::number(position));

    QListWidgetItem *itm = new QListWidgetItem(QString::number(position));
    itm->setIcon(QIcon(QPixmap::fromImage(currentImage).scaled(100,100,Qt::KeepAspectRatio, Qt::FastTransformation)));
    thumbnailList->addItem(itm);

    return 0;
}

int MainWindow::displayTwoImageInPosition(int position)
{
    qInfo() << "position: " << position;
    if (position < 0 || position > (comic->getPageCount()-1))
        return -1;
    if (position == 0 || position == (comic->getPageCount()-1)) {
        displayImageInPosition(position);
        return 0;
    }

    position = (position%2 == 0) ? position : position-1;

    Image* nextImage1 = comic->getPageInPosition(position);
    Image* nextImage2 = comic->getPageInPosition(position+1);
    QByteArray ba1 = nextImage1->getBA();
    QByteArray ba2 = nextImage2->getBA();
    QBuffer qbuff1(&ba1);
    QBuffer qbuff2(&ba2);
    QImageReader qimg1;
    QImageReader qimg2;
    qimg1.setDecideFormatFromContent(true);
    qimg1.setDevice(&qbuff1);
    qimg2.setDecideFormatFromContent(true);
    qimg2.setDevice(&qbuff2);
    qInfo("can read 1: %s", (qimg1.canRead()) ? "yes" : "no");
    qInfo("can read 2: %s", (qimg2.canRead()) ? "yes" : "no");
    QImage firstImage = qimg1.read();
    QImage secondImage = qimg2.read();
    QPixmap *pixmap = new QPixmap(firstImage.width()+secondImage.width()+3, (firstImage.height() > secondImage.height()) ? firstImage.height() : secondImage.height());
    pixmap->fill(Qt::transparent);
    QPainter *painter = new QPainter(pixmap);
    painter->drawPixmap(0, 0, firstImage.width(), firstImage.height(), QPixmap::fromImage(firstImage));
    QPixmap pageSep(3, pixmap->height());
    pageSep.fill(QColor(192, 192, 192, 127));
    painter->drawPixmap(firstImage.width()+1, 0, 3, pixmap->height(), pageSep);
    painter->drawPixmap(firstImage.width()+4, 0, secondImage.width(), secondImage.height(), QPixmap::fromImage(secondImage));
    painter->end();
    qInfo() << "firstImage.height = " << firstImage.width();
    qInfo() << "secondImage.height = " << secondImage.width();
    currentImage = pixmap->toImage();
    comic->setCurrentPage(position);
    normalSize();
    currentPageLabel->setText(QString::number(position)+"-"+QString::number(position+1));

    return 0;
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}
