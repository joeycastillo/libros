/*
 * MVBook for the Open Book
 * The Minimum Viable Book Reader
 * by Joey Castillo
 * 
 * MIT License
 */
#include <MenuSystem.h>
#include "OpenBook.h"

void doLock();

// global reference to the Open Book hardware abstraction
OpenBook *book;

// global state
bool fullRefresh = true;
char *currentBook = NULL;
char currentProgressFile[16];
size_t currentLine = 0;
bool atEnd = false;
bool bookNeedsRefresh = false;
uint8_t buttons = 0;
int menuOffset = 0;

#include "bitmaps.h"
#include "menu.h"
#include "readbook.h"

void open_file(MenuComponent* menu_item) {
    currentBook = (char *)menu_item->get_name();

    // create or read progress file, just an int for what line the reader is on
    strcpy(currentProgressFile, currentBook);
    currentProgressFile[strlen(currentProgressFile) - 1] = 'P';
    if (book->getSD()->exists(currentProgressFile)) {
        File f = book->getSD()->open(currentProgressFile, FILE_READ);
        f.read(&currentLine, sizeof(size_t));
        f.close();
    } else {
        currentLine = 2; // start of lines, after title and author
        File f = book->getSD()->open(currentProgressFile, FILE_WRITE);
        f.write((byte *)&currentLine, sizeof(size_t));
        f.close();
    }
    bookNeedsRefresh = true;

    Serial.print("Opening ");
    Serial.println(currentBook);
}

void setup() {
    pinMode(0, OUTPUT);
    digitalWrite(0, HIGH);
    book = new OpenBook();
    Serial.begin(115200);
    Serial.println("MVBook Demo!");

    pinMode(13, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
    pinMode(15, INPUT_PULLUP);
    pinMode(18, INPUT_PULLUP);
    pinMode(19, INPUT_PULLUP);
    pinMode(20, INPUT_PULLUP);
    pinMode(21, INPUT_PULLUP);

#ifdef ARDUINO_ARCH_RP2040
    MbedSPI* SPI0 = new MbedSPI(4, 3, 2);
    MbedSPI* SPI1 = new MbedSPI(12, 11, 10);
    book->configureScreen(-1, 9, 8, 7, 6, SPI1, 300, 400);
    book->configureSD(5, SPI0);
    book->configureBabel(1, SPI0);
#else
    if (!book->getSD()->begin(38)) Serial.println("No SD?");
    book->configureScreen(-1, 39, 40, 41, 42, &SPI, 300, 400);
    book->configureBabel(44);
#endif

    // display splash screen with factory standard waveform. I feel pretty confident 
    // we can use quick mode exclusively, but until I test more, I want to use the
    // factory waveform at least once when we are first getting started.
    OpenBook_IL0398 *display = book->getDisplay();
    display->setRotation(1);
    display->fillScreen(EPD_WHITE);
    display->drawBitmap(0, 0, OpenBookSplash, 400, 300, EPD_BLACK);
    display->display();
    display->setDisplayMode(OPEN_BOOK_DISPLAY_MODE_QUICK);
    display->fillScreen(EPD_WHITE);
    display->display();

    // restore standard rotation going forward
    display->setRotation(0);

    File root = book->getSD()->open("/");

    File entry = root.openNextFile();
    Serial.println("Adding files...");
    while (entry) {
        if (!entry.isDirectory()) {
            uint64_t magic = 0;
            entry.read((void *)&magic, 8);
            if (magic == 5426643222204338255) { // the string "OPENBOOK"
              char *filename = (char *)malloc(128);
              entry.getName(filename, 128);
              Serial.println(filename);
              MenuItem *item = new MenuItem(filename, &open_file);
              ms.get_root_menu().add_item(item);
            }
        }
        entry = root.openNextFile();
    }

    ms.get_root_menu().set_name("The Open Book");
    ms.display();
}

void loop() {
    buttons = 0;
#ifdef ARDUINO_ARCH_RP2040
    if (digitalRead(14) == 0) buttons |= OPENBOOK_BUTTONMASK_SELECT;
    if (digitalRead(15) == 0) buttons |= OPENBOOK_BUTTONMASK_NEXT;
    if (digitalRead(18) == 0) buttons |= OPENBOOK_BUTTONMASK_PREVIOUS;
    if (digitalRead(13) == 0) buttons |= OPENBOOK_BUTTONMASK_LEFT;
    if (digitalRead(20) == 0) buttons |= OPENBOOK_BUTTONMASK_RIGHT;
    if (digitalRead(19) == 0) buttons |= OPENBOOK_BUTTONMASK_UP;
    if (digitalRead(21) == 0) buttons |= OPENBOOK_BUTTONMASK_DOWN;
    if (digitalRead(12) == 0) buttons |= OPENBOOK_BUTTONMASK_LOCK;
#else
#endif
    if (buttons & OPENBOOK_BUTTONMASK_LOCK) {
        doLock();
    } else if(currentBook == NULL) {
        doMenu();
    } else {
        doReader();
    }
}


void doLock() {
    OpenBook_IL0398 *display = book->getDisplay();

    // display a pretty screen when turned off.
    display->setDisplayMode(OPEN_BOOK_DISPLAY_MODE_GRAYSCALE);
    display->setRotation(1);
    display->fillScreen(EPD_WHITE);
    display->display();
    display->displayGrayscale(0, 0, LockScreen, 400, 300);

    // this hint works to turn the device off, but will also remind the
    // user to turn the device back on when they pick it up next.
    book->getDisplay()->setCursor(248, 290);
    book->getDisplay()->setTextColor(EPD_WHITE, EPD_BLACK);
    book->getDisplay()->setTextSize(1);
    book->getDisplay()->println("Slide the power switch ->");
    display->display();
}
