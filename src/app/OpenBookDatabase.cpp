#include "OpenBookDatabase.h"
#include "OpenBookDevice.h"
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

bool _fileLooksLikeBook(File entry) {
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
        if (_fileLooksLikeBook(entry)) {
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
        if (_fileLooksLikeBook(entry)) {
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
                    case 170732845: // ---\n, end of front matter
                        done = true;
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
