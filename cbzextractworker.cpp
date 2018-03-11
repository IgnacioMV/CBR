#include "cbzextractworker.h"
#include <archive.h>
#include <archive_entry.h>
#include <QMimeDatabase>
#include <QBuffer>
#include <QImageReader>
#include "image.h"
#include "bmp.h"
#include "jpeg.h"
#include "png.h"
#include <QDebug>

CBZExtractWorker::CBZExtractWorker()
{
    this->run = true;
}

void CBZExtractWorker::setFilename(const QString filename)
{
    this->filename = filename;
    this->run = true;
}

void CBZExtractWorker::start()
{
    if (filename == NULL) {
        emit finished();
        return;
    }

    QByteArray ba;
    bool isLeft = true;

    struct archive *arch;
    struct archive_entry *entry;
    int r;
    arch = archive_read_new();
    archive_read_support_filter_all(arch);
    archive_read_support_format_all(arch);
    r = archive_read_open_filename(arch, filename.toStdString().c_str(), 10240);
    if (r != ARCHIVE_OK)
        exit(1);
    Image* image;
    while (archive_read_next_header(arch, &entry) == ARCHIVE_OK && run) {

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

        image = Image::make_image(mime);

        if (!image)
            continue;
        QBuffer qbuff(&ba);
        QImageReader qimgr;
        qimgr.setDecideFormatFromContent(true);
        qimgr.setDevice(&qbuff);
        QImage qimage = qimgr.read();

        image->setFilename(archive_entry_pathname(entry));
        image->setQImage(qimage);
        image->setWidth(qimage.width());
        image->setHeight(qimage.height());
        bool doublePage = (qimage.width() > qimage.height());
        image->setDoublePage(doublePage);
        if (image->isDoublePage()) {
            isLeft = false;
        }
        else {
            isLeft = !isLeft;
            image->setLeftPage(isLeft);
        }
        emit pageExtracted(image);
        emit pageReady();
        ba.clear();
    }
    archive_read_data_skip(arch);
    r = archive_read_free(arch);
    if (r != ARCHIVE_OK)
        exit(1);

    emit finished();
}
