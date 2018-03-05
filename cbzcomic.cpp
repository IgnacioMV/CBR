#include <cbzcomic.h>
#include <comic.h>
#include <image.h>
#include <bmp.h>
#include <jpeg.h>
#include <png.h>

int CBZComic::extract() {
    QByteArray ba;
    int maxPage = 0;
    QList<Image*> pages;

    struct archive *arch;
    struct archive_entry *entry;
    int r;
    arch = archive_read_new();
    archive_read_support_filter_all(arch);
    archive_read_support_format_all(arch);
    r = archive_read_open_filename(arch, Comic::getFilename().toStdString().c_str(), 10240);
    if (r != ARCHIVE_OK)
        exit(1);
    Image* image;
    while (archive_read_next_header(arch, &entry) == ARCHIVE_OK) {
        qInfo("extracting %s",archive_entry_pathname(entry));

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
        //qInfo() << "mime.suffixes " << mime.suffixes();        // Known suffixes for this MIME type ("mp3", "mpga").
        //qInfo() << "mime.preferredSuffix: " << mime.preferredSuffix(); // Preferred suffix for this MIME type ("mp3").com

        if (mime.name().indexOf("image/png") != -1) {
            maxPage += 1;
            image = new PNG();
        }
        else if (mime.name().indexOf("image/jpeg") != -1){
            maxPage += 1;
            image = new JPEG();
        }
        else if (mime.name().indexOf("image/bmp") != -1){
            maxPage += 1;
            image = new BMP();
        }
        else
            continue;

        image->setFilename(archive_entry_pathname(entry));
        qInfo() << "mime.name: " << mime.name();            // Name of the MIME type ("audio/mpeg").
        image->setBA(ba);
        pages.append(image);
        ba.clear();
    }
    setPageCount(maxPage);
    setPages(pages);
    archive_read_data_skip(arch);
    r = archive_read_free(arch);
    if (r != ARCHIVE_OK)
        exit(1);
    //for(int i = 0; i < pages.size(); i++) {
    //    qInfo() << "filename: " << getPages().value(i).getFilename();
    //}
    sortPages();
    return 0;
}
