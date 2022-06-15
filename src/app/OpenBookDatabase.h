#ifndef OpenBookDatabase_h
#define OpenBookDatabase_h

#include <stdint.h>
#include <string>

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
  uint16_t version = OPEN_BOOK_DATABASE_VERSION;
  uint16_t numFields = OPEN_BOOK_NUM_FIELDS;
  uint32_t numBooks = 0;
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
protected:
  uint16_t numFields = 0;
  uint32_t numBooks = 0;
private:
    OpenBookDatabase();
};

#endif // OpenBookDatabase_h
