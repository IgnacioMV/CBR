#include "mainwindow.h"
#include "ui_mainwindow.h"


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


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {

    QWidget *centralWidget = new QWidget;
    this->setCentralWidget(centralWidget);
    QVBoxLayout *centralLayout = new QVBoxLayout(centralWidget);

    scaleFactor = 1.0;
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
    QPushButton *firstPageButton = new QPushButton("First");
    QPushButton *previousPageButton = new QPushButton("Previous");
    QPushButton *nextPageButton = new QPushButton("Next");
    QPushButton *lastPageButton = new QPushButton("Last");

    QObject::connect(nextPageButton, SIGNAL(clicked()), this, SLOT(nextPage()));
    QObject::connect(previousPageButton,SIGNAL(clicked()), this, SLOT(previousPage()));
    QObject::connect(firstPageButton,SIGNAL(clicked()), this, SLOT(firstPage()));
    QObject::connect(lastPageButton,SIGNAL(clicked()), this, SLOT(lastPage()));

    btnLayout->addWidget(firstPageButton);
    btnLayout->addWidget(previousPageButton);
    btnLayout->addWidget(nextPageButton);
    btnLayout->addWidget(lastPageButton);

    centralLayout->addWidget(scrollArea);
    centralLayout->addLayout(btnLayout);

    createActions();
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
            if (wheelEvent->delta() > 0 && scaleFactor < 3.0)
                zoomIn();
            else if(wheelEvent->delta() < 0 && scaleFactor > 0.333)
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

    zoomInAct = viewMenu->addAction(tr("Zoom &In (25%)"), this, &MainWindow::zoomIn);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);

    zoomOutAct = viewMenu->addAction(tr("Zoom &Out (25%)"), this, &MainWindow::zoomOut);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);

    normalSizeAct = viewMenu->addAction(tr("&Normal Size"), this, &MainWindow::normalSize);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
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
bool MainWindow::compareImages(const Image &i1, const Image &i2) {
    int x = QString::compare(i1.getFilename(), i2.getFilename(), Qt::CaseInsensitive);
    if (x < 0)
        return true;
    else
        return false;

}

void MainWindow::openFile()
{
    currentPage = 0;
    maxPage = 0;
    QString fileName = QFileDialog::getOpenFileName(this, "Open the file", "/home/nacho/CBR_old", tr("Comic files (*.cbr *.cbz)"));

    if(fileName.isEmpty() || fileName.isNull()){
        return;
    }
    imageList.clear();

    QByteArray ba;
    currentFile = "";

    struct archive_entry *entry;
    int r;
    arch = archive_read_new();
    archive_read_support_filter_all(arch);
    archive_read_support_format_all(arch);
    r = archive_read_open_filename(arch, fileName.toStdString().c_str(), 10240);
    if (r != ARCHIVE_OK)
        exit(1);
    while (archive_read_next_header(arch, &entry) == ARCHIVE_OK) {
        qInfo("%s",archive_entry_pathname(entry));

        int ret;
        const void *buff;
        size_t size;
        off_t offset;

        for (;;) {
            ret = archive_read_data_block(arch, &buff, &size, &offset);
            ba.append((char *)buff, size);
            if (ret == ARCHIVE_EOF){
                break;
            }
        }

        QMimeDatabase db;
        QMimeType mime = db.mimeTypeForData(ba);
        //qInfo() << "mime.name: " << mime.name();            // Name of the MIME type ("audio/mpeg").
        //qInfo() << "mime.suffixes " << mime.suffixes();        // Known suffixes for this MIME type ("mp3", "mpga").
        //qInfo() << "mime.preferredSuffix: " << mime.preferredSuffix(); // Preferred suffix for this MIME type ("mp3").com

        if (mime.name().indexOf("image/png") != -1) {
            maxPage += 1;
        }
        else if (mime.name().indexOf("image/jpeg") != -1){
            maxPage += 1;
        }
        else if (mime.name().indexOf("image/bmp") != -1){
            maxPage += 1;
        }
        else
            continue;

        Image image;
        image.setFilename(archive_entry_pathname(entry));
        image.setBA(ba);
        imageList.append(image);
        ba.clear();
    }

    archive_read_data_skip(arch);
    r = archive_read_free(arch);
    if (r != ARCHIVE_OK)
        exit(1);

    qSort(imageList.begin(), imageList.end(), MainWindow::compareImages);
    qInfo() << "imageList size: " << imageList.size();
    for(int i = 0; i < imageList.size(); i++) {
        qInfo() << "filename: " << imageList.value(i).getFilename();
    }
    displayImageInPosition(currentPage);
}

void MainWindow::closeFile() {
    maxPage = 0;
    currentPage = 0;
    imageList.clear();
    imageLabel->setPixmap(QPixmap());
    qInfo() << "file closed";
}

void MainWindow::zoomIn() {
    scaleImage(1.25);
}

void MainWindow::zoomOut() {
    scaleImage(0.8);
}

void MainWindow::normalSize() {
    imageLabel->adjustSize();
    zoomInAct->setEnabled(true);
    zoomOutAct->setEnabled(true);
    scaleFactor = 1.0;
}

void MainWindow::scaleImage(double factor)
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 0.333);

}

void MainWindow::nextPage() {
    displayImageInPosition(currentPage+1);
}

void MainWindow::previousPage() {
    displayImageInPosition(currentPage-1);
}

void MainWindow::firstPage(){
    displayImageInPosition(0);
}

void MainWindow::lastPage(){
    int lastPage = imageList.size()-1;
    displayImageInPosition(lastPage);
}

int MainWindow::displayImageInPosition(int position) {
    if (position < 0 || position > (maxPage-1))
        return -1;
    Image nextImage = imageList.value(position);
    qInfo() << "appended ba size: " << nextImage.getBA().size();
    QByteArray ba = nextImage.getBA();
    QBuffer qbuff(&ba);
    QImageReader qimg;
    qimg.setDecideFormatFromContent(true);
    qimg.setDevice(&qbuff);
    qInfo("%i",qimg.canRead());
    currentImage = qimg.read();
    int w = scrollArea->width();
    int h = scrollArea->height();
    imageLabel->setPixmap(QPixmap::fromImage(currentImage).scaled(w,h,Qt::KeepAspectRatio));
    currentPage = position;
    normalSize();
    return 0;
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}
