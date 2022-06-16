#ifndef OpenBookDatabase_h
#define OpenBookDatabase_h

#include <stdint.h>
#include <string>
#include "OpenBookDevice.h"

#define OPEN_BOOK_LIBRARY_FILENAME ("_LIBRARY")
#define OPEN_BOOK_BACKUP_FILENAME ("_LIBBACK")
#define OPEN_BOOK_WORKING_FILENAME ("_LIBTEMP")

#define OPEN_BOOK_DATABASE_VERSION (0x0000)
#define OPEN_BOOK_NUM_FIELDS (5)
#define OPEN_BOOK_TITLE_INDEX (0)
#define OPEN_BOOK_AUTHOR_INDEX (1)
#define OPEN_BOOK_GENRE_INDEX (2)
#define OPEN_BOOK_DESCRIPTION_INDEX (3)
#define OPEN_BOOK_LANGUAGE_INDEX (4)

// Structs for the _LIBRARY database

typedef struct {
    uint32_t tag = 0;
    uint16_t loc = 0;
    uint16_t len = 0;
} BookField;

typedef struct {
    char filename[128];
    uint64_t fileHash = 0;
    uint64_t fileSize = 0;
    uint64_t textStart = 0;
    uint64_t currentPosition = 0;
    uint64_t flags = 0;
    BookField metadata[OPEN_BOOK_NUM_FIELDS];
} BookRecord;

typedef struct {
    uint64_t flags = 0;
    uint32_t version = OPEN_BOOK_DATABASE_VERSION;
    uint16_t numFields = OPEN_BOOK_NUM_FIELDS;
    uint16_t reserved1 = 0;
    uint32_t numBooks = 0;
    uint32_t reserved2 = 0;
    uint64_t reserved3 = 0;
} BookDatabaseHeader;

// structs for the .pag pagination files

typedef struct {
    uint64_t magic = 4992030523817504768;   // for identifying the file
    uint32_t numChapters = 0;               // Number of chapter descriptors
    uint32_t numPages = 0;                  // Number of page descriptors
    uint32_t tocStart = 0;                  // Start of chapter descriptors
    uint32_t pageStart = 0;                 // Start of page descriptors
} BookPaginationHeader;

typedef struct {
    uint32_t loc;       // Location in the text file of the RS indicating chapter separation
    uint16_t len;       // Length of the chapter header, including RS character
    uint16_t reserved;  // Reserved for future use
} BookChapter;

typedef struct {
    uint32_t loc;                           // Location in the text file of the page
    uint16_t len;                           // Length of the page in characters
    struct {
        uint16_t isChapterSeparator : 1;    // 1 if this is a chapter separator page
        uint16_t activeShifts : 2;          // 0-3 for number of format shifts
        uint16_t reserved : 13;             // Reserved for future use
    } flags;
} BookPage;

class OpenBookDatabase {
public:
    static OpenBookDatabase *sharedInstance() {
        static OpenBookDatabase instance;
        return &instance;
    }
    OpenBookDatabase(OpenBookDatabase const&) = delete;
    void operator=(OpenBookDatabase const&) = delete;

    // Methods for working with the main _LIBRARY file
    bool connect();
    bool scanForNewBooks();
    uint32_t getNumberOfBooks();
    BookRecord getBookRecord(uint32_t i);
    std::string getBookTitle(BookRecord record);
    std::string getBookAuthor(BookRecord record);
    std::string getBookDescription(BookRecord record);

    // Methods for dealing with .pag sidecar files
    bool bookIsPaginated(BookRecord record);
    void paginateBook(BookRecord record);
    uint32_t numPages(BookRecord record);

    std::string getBookPage(BookRecord record, uint32_t page);
protected:
    bool _fileLooksLikeBook(File entry);
    std::string _getMetadataAtIndex(BookRecord record, uint16_t i);
    bool _getPaginationFile(BookRecord record, char *outFilename);
    uint16_t numFields = 0;
    uint32_t numBooks = 0;
private:
    OpenBookDatabase();
};

#endif // OpenBookDatabase_h
