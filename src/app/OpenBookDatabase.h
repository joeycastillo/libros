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

typedef struct {
    uint32_t tag = 0;
    uint16_t loc = 0;
    uint16_t len = 0;
} BookField;

typedef struct {
    char filename[128];
    uint64_t fileHash = 0;
    uint64_t fileSize = 0;
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

class OpenBookDatabase {
public:
    static OpenBookDatabase *sharedInstance() {
        static OpenBookDatabase instance;
        return &instance;
    }
    OpenBookDatabase(OpenBookDatabase const&) = delete;
    void operator=(OpenBookDatabase const&) = delete;

    bool connect();
    bool scanForNewBooks();
    uint32_t getNumberOfBooks();
    BookRecord getBookRecord(uint32_t i);
    std::string getBookTitle(BookRecord record);
    std::string getBookAuthor(BookRecord record);
    std::string getBookDescription(BookRecord record);

    bool bookIsPaginated(BookRecord record);
protected:
    bool _fileLooksLikeBook(File entry);
    std::string _getMetadataAtIndex(BookRecord record, uint16_t i);
    uint16_t numFields = 0;
    uint32_t numBooks = 0;
private:
    OpenBookDatabase();
};

#endif // OpenBookDatabase_h