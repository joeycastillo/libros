#include "OpenBookDatabase.h"
#include "sha256.h"
#include <map>

static const uint64_t DATABASE_FILE_IDENTIFIER = 6825903261955698688;

OpenBookDatabase::OpenBookDatabase() {

}

bool OpenBookDatabase::connect() {
    OpenBookDevice *device = OpenBookDevice::sharedInstance();
    BookDatabaseHeader header;

    if (!device->fileExists(OPEN_BOOK_LIBRARY_FILENAME)) {
        if (device->fileExists(OPEN_BOOK_BACKUP_FILENAME)) {
            device->renameFile(OPEN_BOOK_BACKUP_FILENAME, OPEN_BOOK_LIBRARY_FILENAME);
        } else {
            File database = device->openFile(OPEN_BOOK_LIBRARY_FILENAME, O_CREAT | O_RDWR);
            database.write((byte *)&DATABASE_FILE_IDENTIFIER, sizeof(DATABASE_FILE_IDENTIFIER));
            database.write((byte *)&header, sizeof(header));
            database.flush();
            database.close();
        }
    }
    if (device->fileExists(OPEN_BOOK_WORKING_FILENAME)) {
        device->removeFile(OPEN_BOOK_WORKING_FILENAME);
    }
    if (device->fileExists(OPEN_BOOK_BACKUP_FILENAME)) {
        device->removeFile(OPEN_BOOK_BACKUP_FILENAME);
    }
    File database = device->openFile(OPEN_BOOK_LIBRARY_FILENAME);
    uint64_t magic;
    database.read((byte *)&magic, sizeof(magic));
    database.read((byte *)&header, sizeof(BookDatabaseHeader));
    database.close();

    if (magic != DATABASE_FILE_IDENTIFIER) {
        return false;
    }

    if (header.version != OPEN_BOOK_DATABASE_VERSION) {
        return false;
    }

    this->numBooks = header.numBooks;
    this->numFields = header.numFields;

    return true;
}

bool OpenBookDatabase::_fileLooksLikeBook(File entry) {
    uint32_t magic = 0;
    uint32_t extension = 0;
    char filename[128];

    if (entry.isDirectory()) return false;

    entry.getName(filename, 128);
    memcpy((byte *)&extension, filename + (strlen(filename) - 4), 4);
    entry.read((void *)&magic, sizeof(magic));
    // return true if file extension is .txt, and
    // file begins with three hyphens followed by a newline
    return (extension == 1954051118 && magic == 170732845);
}

bool OpenBookDatabase::scanForNewBooks() {
    OpenBookDevice *device = OpenBookDevice::sharedInstance();
    uint32_t numBooks = 0;
    File root, entry;
    SHA256 sha256;
    std::string hash;

    // TODO: scan the existing database for book progress and store it in a map

    // Next, scan the root folder for things that look like books.
    root = device->openFile("/");
    entry = root.openNextFile();
    while (entry) {
        if (this->_fileLooksLikeBook(entry)) {
            numBooks++;
        }
        entry.close();
        entry = root.openNextFile();
    }
    entry.close();

    // now that we have a number of books we can write the header to the temp database
    BookDatabaseHeader header;
    header.numBooks = numBooks;
    File temp = device->openFile(OPEN_BOOK_WORKING_FILENAME, O_RDWR | O_CREAT);
    temp.write((byte *)&DATABASE_FILE_IDENTIFIER, sizeof(DATABASE_FILE_IDENTIFIER));
    temp.write((byte *)&header, sizeof(BookDatabaseHeader));
    temp.flush();
    temp.close();

    root = device->openFile("/");
    entry = root.openNextFile();
    while (entry) {
        if (this->_fileLooksLikeBook(entry)) {
            BookRecord record = {0};
            entry.getName(record.filename, 128);
            hash = sha256(std::string(record.filename));
            memcpy((void *)&record.fileHash, hash.c_str(), sizeof(record.fileHash));
            record.fileSize = entry.size();
            record.currentPosition = 0; // TODO: copy from map
            uint32_t tag;
            char c;
            bool done = false;
            entry.seekSet(4);
            while(!done) {
                entry.read((byte *)&tag, sizeof(tag));
                if (tag == 170732845) { // ---\n, end of front matter
                    done = true;
                    record.textStart = entry.position();
                    break;
                }
                do {
                    c = entry.read();
                } while (c != ':');
                do {
                    c = entry.read();
                } while (c == ' ');
                uint64_t loc = entry.position() - 1;
                uint64_t len = 0;
                do {
                    len++;
                    c = entry.read();
                } while (c != '\n');
                // len is now the length of the metadata
                BookField field;
                field.tag = tag;
                field.loc = loc;
                field.len = len;
                switch (tag) {
                    case 1280592212: // TITL
                        record.metadata[OPEN_BOOK_TITLE_INDEX] = field;
                        break;
                    case 1213486401: // AUTH
                        record.metadata[OPEN_BOOK_AUTHOR_INDEX] = field;
                        break;
                    case 1163021895: // GNRE
                        record.metadata[OPEN_BOOK_GENRE_INDEX] = field;
                        break;
                    case 1129530692: // DESC
                        record.metadata[OPEN_BOOK_DESCRIPTION_INDEX] = field;
                        break;
                    case 1196310860: // LANG
                        record.metadata[OPEN_BOOK_LANGUAGE_INDEX] = field;
                        break;
                    default:
                        break;
                }
            }
            entry.close();

            temp = device->openFile(OPEN_BOOK_WORKING_FILENAME, O_RDWR | O_AT_END);
            temp.write((byte *)&record, sizeof(BookRecord));
            temp.flush();
            temp.close();
        }
        entry.close();
        entry = root.openNextFile();
    }
    entry.close();

    device->renameFile(OPEN_BOOK_LIBRARY_FILENAME, OPEN_BOOK_BACKUP_FILENAME);
    device->renameFile(OPEN_BOOK_WORKING_FILENAME, OPEN_BOOK_LIBRARY_FILENAME);
    device->removeFile(OPEN_BOOK_BACKUP_FILENAME);
    
    return true;
}

uint32_t OpenBookDatabase::getNumberOfBooks() {
    return this->numBooks;
}

BookRecord OpenBookDatabase::getBookRecord(uint32_t i) {
    BookRecord retval;
    File database = OpenBookDevice::sharedInstance()->openFile(OPEN_BOOK_LIBRARY_FILENAME);

    database.seekSet(sizeof(DATABASE_FILE_IDENTIFIER) + sizeof(BookDatabaseHeader) + i * sizeof(BookRecord));
    database.read((byte *)&retval, sizeof(BookRecord));
    database.close();

    return retval;
}

std::string OpenBookDatabase::getBookTitle(BookRecord record) {
    return this->_getMetadataAtIndex(record, OPEN_BOOK_TITLE_INDEX);
}

std::string OpenBookDatabase::getBookAuthor(BookRecord record) {
    return this->_getMetadataAtIndex(record, OPEN_BOOK_AUTHOR_INDEX);
}

std::string OpenBookDatabase::getBookDescription(BookRecord record) {
    return this->_getMetadataAtIndex(record, OPEN_BOOK_DESCRIPTION_INDEX);
}

std::string OpenBookDatabase::_getMetadataAtIndex(BookRecord record, uint16_t i) {
    BookField field = record.metadata[i];
    char *value = (char *)malloc(field.len + 1);
    File f = OpenBookDevice::sharedInstance()->openFile(record.filename);

    f.seekSet(field.loc);
    f.read((void *)value, field.len);
    f.close();
    value[field.len] = 0;
    std::string retval = std::string(value);
    free(value);

    return retval;
}

bool OpenBookDatabase::bookIsPaginated(BookRecord record) {
    char paginationFilename[128];
    return this->_getPaginationFile(record, paginationFilename);
}

// Branchless UTF-8 decoder by Chris Wellons
// Public domain
// https://nullprogram.com/blog/2017/10/06/
// NOTE: the optimizations here are kind of wasted on us since we have 
// a TON of conditionals right after doing this, but oh well.
static void * _utf8_decode(unsigned char *buf, uint32_t *c, int *e) {
    static const char lengths[] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 0
    };
    static const int masks[]  = {0x00, 0x7f, 0x1f, 0x0f, 0x07};
    static const uint32_t mins[] = {4194304, 0, 128, 2048, 65536};
    static const int shiftc[] = {0, 18, 12, 6, 0};
    static const int shifte[] = {0, 6, 4, 2, 0};

    unsigned char *s = buf;
    int len = lengths[s[0] >> 3];

    /* Compute the pointer to the next character early so that the next
     * iteration can start working on the next character. Neither Clang
     * nor GCC figure out this reordering on their own.
     */
    unsigned char *next = s + len + !len;

    /* Assume a four-byte character and load four bytes. Unused bits are
     * shifted out.
     */
    *c  = (uint32_t)(s[0] & masks[len]) << 18;
    *c |= (uint32_t)(s[1] & 0x3f) << 12;
    *c |= (uint32_t)(s[2] & 0x3f) <<  6;
    *c |= (uint32_t)(s[3] & 0x3f) <<  0;
    *c >>= shiftc[len];

    /* Accumulate the various error conditions. */
    *e  = (*c < mins[len]) << 6; // non-canonical encoding
    *e |= ((*c >> 11) == 0x1b) << 7;  // surrogate half?
    *e |= (*c > 0x10FFFF) << 8;  // out of range?
    *e |= (s[1] & 0xc0) >> 2;
    *e |= (s[2] & 0xc0) >> 4;
    *e |= (s[3]       ) >> 6;
    *e ^= 0x2a; // top two bits of each tail byte correct?
    *e >>= shifte[len];

    return next;
}

void OpenBookDatabase::paginateBook(BookRecord record) {
    OpenBookDevice *device = OpenBookDevice::sharedInstance();
    BookPaginationHeader header;
    File paginationFile;
    char paginationFilename[128];

    // to start with, write the empty header just as a placeholder
    if (this->_getPaginationFile(record, paginationFilename)) {
        paginationFile = device->openFile(paginationFilename, O_RDWR | O_TRUNC);
    } else {
        paginationFile = device->openFile(paginationFilename, O_CREAT | O_RDWR);
    }
    paginationFile.write((byte *)&header, sizeof(BookPaginationHeader));
    paginationFile.seekSet(0);
    paginationFile.flush();
    paginationFile.close();

    // now process the whole file and seek out chapter headings.
    BookChapter chapter;
    File f = device->openFile(record.filename);
    f.seekSet(record.textStart);
    do {
        if (f.read() == 0x1e) {
            chapter.loc = f.position() - 1;
            chapter.len++;
            header.numChapters++;
            while(f.read() != '\n') chapter.len++;
            f.close();
            paginationFile = device->openFile(paginationFilename, O_RDWR | O_AT_END);
            paginationFile.write((byte *)&chapter, sizeof(BookChapter));
            paginationFile.flush();
            paginationFile.close();
            f = device->openFile(record.filename);
            f.seekSet(chapter.loc + chapter.len);
            chapter = {0};
        }
    } while (f.available());
    f.close();

    // if we found chapters, mark the TOC as starting right after the header.
    if (header.numChapters) header.tocStart = sizeof(BookPaginationHeader);

    // OKAY! Time to do pages. For this we have to traverse the whole file again,
    // but this time we need to simulate actually laying it out.
    BookPage page;
    union {
        uint32_t value;
        unsigned char buf[4];
    } data;
    uint32_t codepoint;
    int error;
    BabelDevice *babel = device->getTypesetter()->getBabel();

    f = device->openFile(record.filename);
    f.seekSet(record.textStart);
    f.read(data.buf, 4);
    uint16_t x = 0;
    uint16_t y = 0;
    page.loc = f.position();
    page.len = 1; // we read four bytes right at the outset
    header.numPages = 1;
    data.buf[0] = f.read();
    // this is ugly but i'm moving fast. if high bit is not set, this is a single byte
    if (data.buf[0] & 0b01111111) {
        // from here we just read in more bytes as needed.
        // this guarantees we only read in one codepoint at the outset.
        if ((data.buf[0] & 0b11000000) == 0b11000000) {
            data.buf[1] = f.read();
            page.len++;
        }
        if ((data.buf[0] & 0b11100000) == 0b11100000) {
            data.buf[2] = f.read();
            page.len++;
        }
        if ((data.buf[0] & 0b11110000) == 0b11110000) {
            data.buf[3] = f.read();
            page.len++;
        }
    }
    do {
        void *next = _utf8_decode(data.buf, &codepoint, &error);
        int8_t shift = (uint32_t) next - (uint32_t)&(data.buf);
        page.len += shift;
        data.value >>= shift * 8;
        f.read(data.buf + 4 - shift, shift);
        uint32_t info = babel->fetch_glyph_basic_info((BABEL_CODEPOINT)codepoint);
        x += BABEL_INFO_GET_GLYPH_WIDTH(info);
        if (x > 288) {
            x = 0;
            y += 16;
        }
        if (y > 368) {
            x = 0;
            y = 0;
            f.close();
            paginationFile = device->openFile(paginationFilename, O_RDWR | O_AT_END);
            paginationFile.write((byte *)&page, sizeof(BookPage));
            paginationFile.flush();
            paginationFile.close();
            f = device->openFile(record.filename);
            f.seekSet(page.loc + page.len);
            page.loc = f.position();
            page.len = 0;
            header.numPages++;
        }
    } while (f.available());
    f.close();

    header.pageStart = header.tocStart + header.numChapters * sizeof(BookChapter);

    paginationFile = device->openFile(paginationFilename, O_RDWR);
    paginationFile.seekSet(0);
    paginationFile.write((byte *)&header, sizeof(BookPaginationHeader));
    paginationFile.flush();
    paginationFile.close();
}

uint32_t OpenBookDatabase::numPages(BookRecord record) {
    char paginationFilename[128];
    if (this->_getPaginationFile(record, paginationFilename)) {
        BookPaginationHeader header;
        File f = OpenBookDevice::sharedInstance()->openFile(paginationFilename);
        f.read(&header, sizeof(BookPaginationHeader));
        return header.numPages;
    }

    return 0;
}

std::string OpenBookDatabase::getBookPage(BookRecord record, uint32_t page) {
    char paginationFilename[128];

    if (this->_getPaginationFile(record, paginationFilename)) {
        BookPaginationHeader header;
        BookPage pageInfo;

        File f = OpenBookDevice::sharedInstance()->openFile(paginationFilename);
        f.read(&header, sizeof(BookPaginationHeader));
        if (page >= header.numPages) return "";

        f.seekSet(header.pageStart + page * sizeof(BookPage));
        f.read(&pageInfo, sizeof(BookPage));
        f.close();
        f = OpenBookDevice::sharedInstance()->openFile(record.filename);
        f.seekSet(pageInfo.loc);
        char *buf = (char *)malloc(pageInfo.len + 1);
        f.read(buf, pageInfo.len);
        f.close();
        buf[pageInfo.len] = 0;
        std::string retval = std::string(buf);
        free(buf);

        return retval;
    }

    return "";
}

bool OpenBookDatabase::_getPaginationFile(BookRecord record, char *outFilename) {
    const uint32_t extension = 1734438958; // four ASCII characters, .pag

    memcpy(outFilename, record.filename, 128);
    memcpy((byte *)outFilename + (strlen(outFilename) - 4), (byte *)&extension, sizeof(extension));

    return OpenBookDevice::sharedInstance()->fileExists(outFilename);
}
