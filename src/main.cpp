#include "OpenBook.h"
#include "bitmaps.h"
#include "OpenBookApplication.h"

// global reference to the Open Book hardware abstraction
OpenBook *book;
OpenBookApplication *application;

void setup() {
    book = new OpenBook();

#ifdef ARDUINO_ARCH_RP2040
    // enable secondary voltage regulator
    pinMode(0, OUTPUT);
    digitalWrite(0, HIGH);

    MbedSPI* SPI0 = new MbedSPI(4, 3, 2);
    MbedSPI* SPI1 = new MbedSPI(12, 11, 10);
    book->configureScreen(-1, 9, 8, 7, 6, SPI1, 300, 400);
    book->configureSD(5, SPI0);
    book->configureBabel(1, SPI0);

    OpenBookButtonConfig buttonConfig;
    buttonConfig.left_pin = 13;
    buttonConfig.down_pin = 21;
    buttonConfig.up_pin = 19;
    buttonConfig.right_pin = 20;
    buttonConfig.select_pin = 14;
    buttonConfig.previous_pin = 18;
    buttonConfig.next_pin = 15;
    buttonConfig.cd_pin = -1;
    buttonConfig.lock_pin = 12;
    book->configureButtons(LOW, buttonConfig);
#else
    book->configureSD(38, &SPI);
    book->configureScreen(-1, 39, 40, 41, 42, &SPI, 300, 400);
    book->configureBabel(44);
#endif

    Window *window = new Window(0, 0, 300, 400);
    application = new OpenBookApplication(window, book);

    Serial.println("Entering loop...");
    application->run();
}

void loop() {
}
