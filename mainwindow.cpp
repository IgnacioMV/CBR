#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QThread>
#include <QObject>
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
#include <QFont>
#include <QCommonStyle>

#include "comic.h"
#include "cbzcomic.h"
#include "thumbnailworker.h"
#include "displaymode.h"
#include "scaleimageworker.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {

    displayMode = DisplayMode::FitToPage;
    scalingAlgorithm = ScalingAlgorithms::Downsampling;
    firstPageShown = false;
    comicOpened = false;
    QWidget *centralWidget = new QWidget;
    this->setCentralWidget(centralWidget);
    QVBoxLayout *centralLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout *midLayout = new QHBoxLayout;

    thumbnailList = new QListWidget;
    thumbnailList->setViewMode(QListView::IconMode);
    thumbnailList->setMovement(QListView::Static);
    thumbnailList->setFixedWidth(200);
    thumbnailList->setStyleSheet("QListWidget{background: #EEEEEE;}");
    connect(thumbnailList, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
                this, SLOT(onThumbnailDoubleClick(QListWidgetItem*)));

    scaleFactor = 1.0;
    twoPage = false;
    scrollArea = new QScrollArea();
    imageLabel = new QLabel();
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true); //It scales the image of the label so that the image fits the Qlabel, not bigger or smaller.
    scrollArea->setWidgetResizable(false);//This property holds whether the scroll area should resize the view widget
    scrollArea->setWidget(imageLabel);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->installEventFilter(this);
    scrollArea->horizontalScrollBar()->installEventFilter(this);
    scrollArea->verticalScrollBar()->installEventFilter(this);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    //firstPageButton = new QPushButton("First");
    //previousPageButton = new QPushButton("Previous");
    //nextPageButton = new QPushButton("Next");
    //lastPageButton = new QPushButton("Last");
    firstPageButton = new QPushButton(QIcon(":images/double_left.png"), "");
    previousPageButton = new QPushButton(QIcon(":images/single_left.png"), "");
    nextPageButton = new QPushButton(QIcon(":images/single_right.png"), "");
    lastPageButton = new QPushButton(QIcon(":images/double_right.png"), "");

    currentPageLabel = new QLabel(" ");
    currentPageLabel->setStyleSheet("QLabel { background-color : white }");
    currentPageLabel->setFixedWidth(55);
    currentPageLabel->setAlignment(Qt::AlignCenter);
    QFont font = currentPageLabel->font();
    font.setPointSize(18);
    font.setBold(true);
    currentPageLabel->setFont(font);

    QObject::connect(nextPageButton, SIGNAL(clicked()), this, SLOT(nextPage()));
    QObject::connect(previousPageButton,SIGNAL(clicked()), this, SLOT(previousPage()));
    QObject::connect(firstPageButton,SIGNAL(clicked()), this, SLOT(firstPage()));
    QObject::connect(lastPageButton,SIGNAL(clicked()), this, SLOT(lastPage()));

    nextPageButton->setShortcut(Qt::Key_PageDown);
    previousPageButton->setShortcut(Qt::Key_PageUp);
    firstPageButton->setShortcut(Qt::Key_Home);
    lastPageButton->setShortcut(Qt::Key_End);

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
    Q_ASSERT(obj);
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

    QMenu *viewModeMenu = viewMenu->addMenu(tr("&View Mode"));
    fitWidthAct = viewModeMenu->addAction(tr("Fit width"), this, &MainWindow::fitWidth);
    fitHeightAct = viewModeMenu->addAction(tr("Fit height"), this, &MainWindow::fitHeight);
    fitToWindowAct = viewModeMenu->addAction(tr("Fit to window"), this, &MainWindow::fitToWindow);
    originalAct = viewModeMenu->addAction(tr("Original"), this, &MainWindow::fitOriginal);

    QMenu *algorithmMenu =viewMenu->addMenu(tr("&Algorithm"));
    downsamplingAct = algorithmMenu->addAction(tr("Downsampling"), this, &MainWindow::downsamplingMode);
    bilinearAct = algorithmMenu->addAction(tr("Bilinear"), this, &MainWindow::bilinearMode);
    bicubicAct = algorithmMenu->addAction(tr("Bicubic"), this, &MainWindow::bicubicMode);


    QCommonStyle* myStyle = new QCommonStyle;
    QIcon chosenIcon = myStyle->standardIcon(QStyle::SP_DialogApplyButton );
    downsamplingAct->setIcon(chosenIcon);
    fitToWindowAct->setIcon(chosenIcon);
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
    if (comicOpened == false) {
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
    currentPageLabel->setText(QString::number(comic->getCurrentPage()));
    thumbnailList->setCurrentRow(comic->getCurrentPage());
}

void MainWindow::openFile()
{

    QString fileName = QFileDialog::getOpenFileName(this, "Open the file", "/home", tr("Comic files (*.cbr *.cbz)"));
    if(fileName.isEmpty() || fileName.isNull()){
        return;
    }
    if (comicOpened) {
        closeFile();
    }

    comic = new CBZComic(fileName);
    comicOpened = true;
    comic->extract(this);
    QPixmap emptyThumbnail(200, 200);
    emptyThumbnail.fill("grey");

    (twoPage) ? displayTwoImageInPosition(comic->getCurrentPage()) : displayImageInPosition(comic->getCurrentPage());
    updatePageActions();    
}

void MainWindow::closeFile()
{
    qInfo() << "closing file...";
    comicOpened = false;
    comic->stopExtracting();
    for (QThread* thread : thumbnailThreads)
        if (thread->isRunning()) {
            thread->exit();
            thread->wait();
            delete thread;
        }
    thumbnailThreads.clear();
    firstPageShown = false;
    delete comic;
    comic = NULL;
    previousImage = QImage();
    currentImage = QImage();
    nextImage = QImage();
    firstImage = QImage();
    lastImage = QImage();

    zoomPixmap = QPixmap();
    previousPixmap = QPixmap();
    currentPixmap = QPixmap();
    nextPixmap = QPixmap();
    lastPixmap = QPixmap();
    firstPixmap = QPixmap();
    updatePageActions();
    imageLabel->setPixmap(QPixmap());
    thumbnailList->clear();
    qInfo() << "file closed";
}

void MainWindow::setThumbnail(const QPixmap &thumbnail, const int i)
{
    if (comicOpened)
           thumbnailList->item(i)->setIcon(thumbnail);
}

void MainWindow::onThumbnailDoubleClick(QListWidgetItem *item)
{
    int row = thumbnailList->currentIndex().row();
    (twoPage) ? displayTwoImageInPosition(row) : displayImageInPosition(row);
    updatePageActions();
}

void MainWindow::twoPages()
{
    int currentPage = comic->getCurrentPage();
    twoPage = twoPagesAct->isChecked();
    if (comic != NULL)
        (twoPage) ? displayTwoImageInPosition(currentPage) : displayImageInPosition(currentPage);
    processNextPixmap();
    processPreviousPixmap();
}

void MainWindow::zoomIn()
{
    scaleImage(1.25);
}

void MainWindow::zoomOut()
{
    scaleImage(0.8);
}

void MainWindow::fitWidth()
{
    changeSelectedDisplayMode(DisplayMode::FitToWidth);

}

void MainWindow::fitHeight()
{
    changeSelectedDisplayMode(DisplayMode::FitToHeight);
}


void MainWindow::fitToWindow()
{
    changeSelectedDisplayMode(DisplayMode::FitToPage);
}

void MainWindow::fitOriginal()
{
    changeSelectedDisplayMode(DisplayMode::Original);
}

void MainWindow::changeSelectedDisplayMode(DisplayMode mode)
{
    displayMode = mode;
    scaleFactor = 1.0;
    zoomInAct->setEnabled(true);
    zoomOutAct->setEnabled(false);
    int width = (displayMode == DisplayMode::Original || displayMode == DisplayMode::FitToHeight) ? 0 : scrollArea->width();
    int height = (displayMode == DisplayMode::Original || displayMode == DisplayMode::FitToWidth) ? 0 : scrollArea->height();
    asyncResizeImageForAlgorithm(comic->getCurrentPage(), width, height, scalingAlgorithm);

    fitWidthAct->setIcon(QIcon());
    fitHeightAct->setIcon(QIcon());
    fitToWindowAct->setIcon(QIcon());
    originalAct->setIcon(QIcon());
    QCommonStyle* myStyle = new QCommonStyle;
    QIcon chosenIcon = myStyle->standardIcon(QStyle::SP_DialogApplyButton );
    switch(mode) {
        case DisplayMode::FitToWidth :
        {
            fitWidthAct->setIcon(chosenIcon);
            break;
        }
    case DisplayMode::FitToHeight :
        {
            fitHeightAct->setIcon(chosenIcon);
            break;
        }
    case DisplayMode::FitToPage :
        {
            fitToWindowAct->setIcon(chosenIcon);
            break;
        }
    case DisplayMode::Original :
        {
            originalAct->setIcon(chosenIcon);
            break;
        }
    }
    if (comicOpened) {
        processNextPixmap();
        processPreviousPixmap();
    }
}

void MainWindow::downsamplingMode()
{
    changeSelectedAlgorithm(ScalingAlgorithms::Downsampling);
}

void MainWindow::bilinearMode()
{
    changeSelectedAlgorithm(ScalingAlgorithms::Bilinear);
}

void MainWindow::bicubicMode()
{
    changeSelectedAlgorithm(ScalingAlgorithms::Bicubic);
}

void MainWindow::changeSelectedAlgorithm(ScalingAlgorithms algorithm)
{
    scalingAlgorithm = algorithm;
    downsamplingAct->setIcon(QIcon());
    bilinearAct->setIcon(QIcon());
    bicubicAct->setIcon(QIcon());
    QCommonStyle* myStyle = new QCommonStyle;
    QIcon chosenIcon = myStyle->standardIcon(QStyle::SP_DialogApplyButton );
    switch(algorithm) {
        case ScalingAlgorithms::Downsampling :
        {
            downsamplingAct->setIcon(chosenIcon);
            break;
        }
        case ScalingAlgorithms::Bilinear :
        {
            bilinearAct->setIcon(chosenIcon);
            break;
        }
        case ScalingAlgorithms::Bicubic :
        {
            bicubicAct->setIcon(chosenIcon);
            break;
        }
        default :
            downsamplingAct->setIcon(chosenIcon);
    }
    if (comicOpened) {
        processNextPixmap();
        processPreviousPixmap();
    }
}

void MainWindow::normalSize()
{
    imageLabel->setPixmap(currentPixmap);
    imageLabel->adjustSize();
    zoomInAct->setEnabled(true);
    zoomOutAct->setEnabled(false);
    scaleFactor = 1.0;
}

void MainWindow::scaleImage(double factor)
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    int w = (displayMode == DisplayMode::Original) ? imageLabel->width() : scrollArea->width();
    int h = (displayMode == DisplayMode::Original) ? imageLabel->height() : scrollArea->height();
    int w1 = imageLabel->width();
    int h1 = imageLabel->height();
    asyncZoomForAlgorithm(w*scaleFactor, h*scaleFactor, scalingAlgorithm);
    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 3.0);
    zoomOutAct->setEnabled(scaleFactor > 1.0);
}

void MainWindow::nextPage()
{
    scaleFactor = 1.0;
    previousPixmap = currentPixmap;
    currentPixmap = nextPixmap;
    nextPixmap = QPixmap();

    previousImage= currentImage;
    currentImage= nextImage;
    nextImage = QImage();

    int currentPosition = comic->getCurrentPage();
    int nextPosition = currentPosition;
    if (twoPage)
        if (comic->getPages().value(currentPosition)->isDoublePage()) {
            nextPosition = currentPosition+1;
        }
        else if (comic->getPages().value(currentPosition)->isLeftPage()) {
            nextPosition = nextPosition+2;
        }
        else {
            nextPosition = nextPosition+1;
        }
    else {
        nextPosition = nextPosition+1;
    }
    if (nextPosition > (comic->getPageCount()-1))
        nextPosition = comic->getPageCount()-1;
    comic->setCurrentPage(nextPosition);

    imageLabel->setPixmap(currentPixmap);
    imageLabel->adjustSize();
    processNextPixmap();
    updatePageActions();
}

void MainWindow::previousPage()
{
    scaleFactor = 1.0;
    nextPixmap = currentPixmap;
    currentPixmap = previousPixmap;
    previousPixmap = QPixmap();

    nextImage = currentImage;
    currentImage = previousImage;
    previousImage= QImage();

    int currentPosition = comic->getCurrentPage();
    int nextPosition = currentPosition;
    if (twoPage && currentPosition != 1)
        if (comic->getPages().value(currentPosition-1)->isDoublePage()) {
            nextPosition = currentPosition-1;
        }
        else  {
            nextPosition = nextPosition-2;
        }
    else
        nextPosition = nextPosition-1;

    if (nextPosition < 0)
        nextPosition = 0;
    comic->setCurrentPage(nextPosition);

    imageLabel->setPixmap(currentPixmap);
    imageLabel->adjustSize();
    processPreviousPixmap();
    updatePageActions();
}

void MainWindow::firstPage()
{
    scaleFactor = 1.0;
    if (firstPixmap.isNull())
        (twoPage) ? displayTwoImageInPosition(0) : this->displayImageInPosition(0);
    else {
        currentPixmap = firstPixmap;
        imageLabel->setPixmap(currentPixmap);
        imageLabel->adjustSize();
    }
    comic->setCurrentPage(0);
    int width = (displayMode == DisplayMode::Original || displayMode == DisplayMode::FitToHeight) ? 0 : scrollArea->width();
    int height = (displayMode == DisplayMode::Original || displayMode == DisplayMode::FitToWidth) ? 0 : scrollArea->height();
    asyncResizeImageForAlgorithm(1, width, height, scalingAlgorithm);
    updatePageActions();
}

void MainWindow::lastPage()
{
    scaleFactor = 1.0;
    int lastPage = comic->getPageCount()-1;
    if (lastPixmap.isNull())
        (twoPage) ? this->displayTwoImageInPosition(lastPage) : this->displayImageInPosition(lastPage);
    else {
        currentPixmap = lastPixmap;
        imageLabel->setPixmap(currentPixmap);
        imageLabel->adjustSize();
    }
    comic->setCurrentPage(lastPage);
    int width = (displayMode == DisplayMode::Original || displayMode == DisplayMode::FitToHeight) ? 0 : scrollArea->width();
    int height = (displayMode == DisplayMode::Original || displayMode == DisplayMode::FitToWidth) ? 0 : scrollArea->height();
    asyncResizeImageForAlgorithm(comic->getPageCount()-2, width, height, scalingAlgorithm);
    updatePageActions();
}

int MainWindow::displayImageInPosition(int position)
{
    if ((position < 0 || position > (comic->getPageCount()-1)) && comic->getPageCount() != 1)
        return -1;
    if (position == 0 && !firstPixmap.isNull())
        imageLabel->setPixmap(firstPixmap);
    else if (position == comic->getPageCount() && !lastPixmap.isNull())
        imageLabel->setPixmap(lastPixmap);
    else if (position == (comic->getCurrentPage()-1) && !previousPixmap.isNull())
        imageLabel->setPixmap(previousPixmap);
    else if (position == (comic->getCurrentPage()+1) && !nextPixmap.isNull())
        imageLabel->setPixmap(nextPixmap);
    else {
        comic->setCurrentPage(position);
        int w = (displayMode == DisplayMode::Original) ? imageLabel->width() : scrollArea->width();
        int h = (displayMode == DisplayMode::Original) ? imageLabel->height() : scrollArea->height();
        int currentPage = comic->getCurrentPage();
        currentPixmap = comic->getPages().value(currentPage)->getPixmapForSize(w, h);
        imageLabel->setPixmap(comic->getPages().value(comic->getCurrentPage())->getPixmapForSize(w, h));
        imageLabel->adjustSize();
    }
    comic->setCurrentPage(position);
    scaleFactor = 1.0;
    currentPageLabel->setText(QString::number(position));

    return 0;
}

int MainWindow::displayTwoImageInPosition(int position)
{
    if (position < 0 || position > (comic->getPageCount()-1))
        return -1;
    if (position == 0 || position == (comic->getPageCount()-1)) {
        displayImageInPosition(position);
        return 0;
    }

    Image* nextImage1 = comic->getPageInPosition(position);
    Image* nextImage2;
    if (nextImage1->isDoublePage())
        return 1;
    else {
        if (nextImage1->isLeftPage())
            nextImage2 = comic->getPageInPosition(position+1);
        else {
            position = position - 1;
            nextImage2 = nextImage1;
            nextImage1 = comic->getPageInPosition(position);
            comic->setCurrentPage(position);
        }
    }
    QPixmap *pixmap = new QPixmap(nextImage1->getWidth()+nextImage2->getWidth()+3, (nextImage1->getHeight() > nextImage2->getHeight()) ? nextImage1->getHeight() : nextImage2->getHeight());
    pixmap->fill(Qt::transparent);
    QPainter *painter = new QPainter(pixmap);
    painter->drawPixmap(0, 0, nextImage1->getWidth(), nextImage1->getHeight(), QPixmap::fromImage(nextImage1->getQImage()));
    QPixmap pageSep(3, pixmap->height());
    pageSep.fill(QColor(192, 192, 192, 127));
    painter->drawPixmap(nextImage1->getWidth()+1, 0, 3, pixmap->height(), pageSep);
    painter->drawPixmap(nextImage1->getWidth()+4, 0, nextImage2->getWidth(), nextImage2->getHeight(), QPixmap::fromImage(nextImage2->getQImage()));
    painter->end();
    int w = (displayMode == DisplayMode::Original) ? imageLabel->width() : scrollArea->width();
    int h = (displayMode == DisplayMode::Original) ? imageLabel->height() : scrollArea->height();
    currentPixmap = (pixmap->scaled(w, h, Qt::KeepAspectRatio));
    imageLabel->setPixmap(currentPixmap);
    imageLabel->adjustSize();
    comic->setCurrentPage(position);
    currentPageLabel->setText(QString::number(position)+"-"+QString::number(position+1));

    processNextPixmap();
    processPreviousPixmap();

    return 0;
}

void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep()/2)));
}

void MainWindow::pageReady() {
    updatePageActions();

    if (comicOpened == false)
        return ;
    int i = comic->getPages().count()-1;
    if (!comic->getPages().value(i))
        return ;
    if (!firstPageShown) {
        (twoPage) ? displayTwoImageInPosition(0) : displayImageInPosition(0);
        currentImage = comic->getPages().value(0)->getQImage();
        updatePageActions();
        firstPageShown = true;
    }

    if (comic->getPageCount() == 2){
        processNextPixmap();
    }
    QPixmap emptyThumbnail(200, 200);
    emptyThumbnail.fill("grey");

    QListWidgetItem *itm = new QListWidgetItem(QString::number(i));
    itm->setTextAlignment(Qt::AlignCenter);
    itm->setSizeHint(QSize(200, 200));
    itm->setIcon(emptyThumbnail);
    thumbnailList->setIconSize(QSize(150,150));
    thumbnailList->addItem(itm);
    QThread *thread = new QThread();
    ThumbnailWorker *tWorker = new ThumbnailWorker();
    tWorker->moveToThread( thread );
    QObject::connect( thread, SIGNAL(started()), tWorker, SLOT(start()) );
    QObject::connect( tWorker, SIGNAL(thumbnailCreated(const QPixmap &, const int &)), this, SLOT(setThumbnail(const QPixmap &, const int &)));
    QObject::connect( tWorker, SIGNAL(finished()), thread, SLOT(quit()));

    tWorker->setW(150);
    tWorker->setH(150);
    tWorker->setI(i);
    tWorker->setSourceImage(comic->getPages().value(i));
    thumbnailThreads.append(thread);
    thread->start();
}

void MainWindow::pixmapReady(const QImage &qimage, const QPixmap &pixmap, const int i)
{
    if (i == comic->getCurrentPage()) {
        currentPixmap = pixmap;
        imageLabel->setPixmap(pixmap);
        imageLabel->adjustSize();
    }
    if (i > comic->getCurrentPage()){
        nextPixmap = pixmap;
        nextImage = qimage;
    }
    if (i == 0) {
        firstPixmap = pixmap;
        firstImage = qimage;
    }
    if (i == comic->getPageCount()) {
        lastPixmap = pixmap;
        lastImage = qimage;
    }
    if (i < comic->getCurrentPage())
    {
        previousPixmap = pixmap;
        previousImage = qimage;
    }
}

void MainWindow::zoomReady(const QPixmap &pixmap) {
    zoomPixmap = pixmap;
    imageLabel->setPixmap(zoomPixmap);
    imageLabel->adjustSize();
}

void MainWindow::finishedExtraction() {
    qInfo() << "finished extraction";
}

void MainWindow::asyncResizeImageForAlgorithm(int i, int width, int height, ScalingAlgorithms algorithm)
{
    QThread *thread = new QThread();
    ScaleImageWorker *siWorker = ScaleImageWorker::make_scaleImageWorker(algorithm);

    siWorker->moveToThread( thread );
    QObject::connect( thread, SIGNAL(started()), siWorker, SLOT(start()) );
    QObject::connect( siWorker, SIGNAL(pixmapReady(const QImage &, const QPixmap &, const int &)), this, SLOT(pixmapReady(const QImage &, const QPixmap &, const int &)));
    QObject::connect( siWorker, SIGNAL(finished()), thread, SLOT(quit()));

    siWorker->setW(width);
    siWorker->setH(height);
    siWorker->setI(i);

    if (i == (comic->getPageCount()-1) || i == 0 || comic->getPages().value(i)->isDoublePage() || !twoPage){
        siWorker->setSourceImages(comic->getPages().value(i), NULL);
    }
    else
        siWorker->setSourceImages(comic->getPages().value(i), comic->getPages().value(i+1));

    thread->start();
}

void MainWindow::asyncZoomForAlgorithm(int width, int height, ScalingAlgorithms algorithm)
{
    QThread *thread = new QThread();
    ScaleImageWorker *siWorker = ScaleImageWorker::make_scaleImageWorker(algorithm);

    siWorker->moveToThread( thread );
    QObject::connect( thread, SIGNAL(started()), siWorker, SLOT(startZoom()) );
    QObject::connect( siWorker, SIGNAL(zoomReady(const QPixmap &)), this, SLOT(zoomReady(const QPixmap &)));
    QObject::connect( siWorker, SIGNAL(finished()), thread, SLOT(quit()));

    Image *img = comic->getPages().value(comic->getCurrentPage());
    QImage qimage = currentImage;
    img->setQImage(qimage);
    img->setWidth(currentImage.width());
    img->setHeight(currentImage.height());

    siWorker->setW(width);
    siWorker->setH(height);
    siWorker->setI(0);
    siWorker->setSourceImages(img, nullptr);
    thread->start();
}

void MainWindow::processNextPixmap()
{
    int currentPosition = comic->getCurrentPage();
    int nextPosition = currentPosition;
    if (twoPage)
        if (comic->getPages().value(currentPosition)->isDoublePage()) {
            nextPosition = currentPosition+1;
        }
        else if (comic->getPages().value(currentPosition)->isLeftPage()) {
            nextPosition = nextPosition+2;
        }
        else {
            nextPosition = nextPosition+1;
        }
    else {
        nextPosition = nextPosition+1;
    }
    if (nextPosition > (comic->getPageCount()-1))
        nextPosition = comic->getPageCount()-1;

    int width = (displayMode == DisplayMode::Original || displayMode == DisplayMode::FitToHeight) ? 0 : scrollArea->width();
    int height = (displayMode == DisplayMode::Original || displayMode == DisplayMode::FitToWidth) ? 0 : scrollArea->height();

    if (nextPosition <= comic->getPageCount()-1){
        asyncResizeImageForAlgorithm(nextPosition, width, height, scalingAlgorithm);
    }
}

void MainWindow::processPreviousPixmap()
{
    int currentPosition = comic->getCurrentPage();
    int nextProcess;
    if (currentPosition == 0)
        return ;
    if (twoPage && currentPosition != 1)
        if (comic->getPages().value(currentPosition-1)->isDoublePage()) {
            nextProcess = currentPosition-1;
        }
        else  {
            nextProcess = currentPosition-2;
        }
    else {
        nextProcess = currentPosition-1;
    }
    int width = (displayMode == DisplayMode::Original || displayMode == DisplayMode::FitToHeight) ? 0 : scrollArea->width();
    int height = (displayMode == DisplayMode::Original || displayMode == DisplayMode::FitToWidth) ? 0 : scrollArea->height();
    if (nextProcess >= 0)
        asyncResizeImageForAlgorithm(nextProcess, width, height, scalingAlgorithm);
}
