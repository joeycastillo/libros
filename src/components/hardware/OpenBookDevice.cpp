#include "OpenBookDevice.h"
#include "OpenBook_IL0398.h"
#include "sleep.h"

OpenBookDevice::OpenBookDevice() {
#ifdef ARDUINO_ARCH_RP2040
    // enable secondary voltage regulator
    pinMode(0, OUTPUT);
    digitalWrite(0, HIGH);

    MbedSPI* SPI0 = new MbedSPI(4, 3, 2);
    MbedSPI* SPI1 = new MbedSPI(12, 11, 10);
    this->configureScreen(-1, 9, 8, 7, 6, SPI1, 300, 400);
    this->configureSD(5, SPI0);
    this->configureBabel(1, SPI0);

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
    this->configureButtons(LOW, buttonConfig);
#else
    book->configureSD(38, &SPI);
    book->configureScreen(-1, 39, 40, 41, 42, &SPI, 300, 400);
    book->configureBabel(44);
#endif
}

/**
 @brief Configures the e-ink screen.
 @param srcs Chip select pin for the SRAM. Pass in OPENBOOK_NOT_PRESENT if you
             have omitted this chip.
 @param ecs Chip select pin for the e-ink screen.
 @param edc Data/command pin for the e-ink screen.
 @param erst Reset pin for the e-ink screen.
 @param ebsy Busy pin for the e-ink screen.
 @param spi Address of the SPI bus for the screen and SRAM chip.
 @param width Native width of the display
 @param height Native height of the display
 @returns always returns true for now
 @note A medium-term term goal is to write our own driver for the screen and
       have it live in this repository. For now, we're using the Adafruit_IL0398
       driver. Refresh rate is honestly a bit slow, and I know this screen can
       refresh faster because the GxEPD2 library is downright snappy. GxEPD2
       also has LUTs for partial refresh, which I think we want to port over.
       Inspo:
        * Adafruit_EPD: https://github.com/adafruit/Adafruit_EPD
        * GxEPD2: https://github.com/ZinggJM/GxEPD2
        * IL0398 Datasheet: https://cdn.sparkfun.com/assets/f/a/9/3/7/4.2in_ePaper_Driver.pdf
*/
bool OpenBookDevice::configureScreen(int8_t srcs, int8_t ecs, int8_t edc, int8_t erst, int8_t ebsy, SPIClass *spi, int width, int height) {
    OpenBook_IL0398 *display = new OpenBook_IL0398(width, height, edc, erst, ecs, srcs, ebsy, spi);
    this->display = display;

    return true;
}

/**
 @brief Configures the buttons.
*/
bool OpenBookDevice::configureButtons(int8_t active, OpenBookButtonConfig config) {
    int8_t pull = active ? INPUT_PULLDOWN : INPUT_PULLUP;
    if (config.left_pin != -1) pinMode(config.left_pin, pull);
    if (config.down_pin != -1) pinMode(config.down_pin, pull);
    if (config.up_pin != -1) pinMode(config.up_pin, pull);
    if (config.right_pin != -1) pinMode(config.right_pin, pull);
    if (config.select_pin != -1) pinMode(config.select_pin, pull);
    if (config.previous_pin != -1) pinMode(config.previous_pin, pull);
    if (config.next_pin != -1) pinMode(config.next_pin, pull);
    if (config.cd_pin != -1) pinMode(config.cd_pin, pull);
    if (config.lock_pin != -1) pinMode(config.lock_pin, pull);

    this->buttonConfig = config;
    this->activeState = active;

    return true;
}

/**
 @brief Configures the buttons for devices with a MCP23008 I2C port expander.
 @param interrupt The port expander's interrupt pin.
 @returns always returns true for now
 @note This is currently the button solution for the wing. All buttons are on
       the port expander, including the Lock button.
*/
bool OpenBookDevice::configureI2CButtons(int8_t active, int8_t interrupt) {
    Adafruit_MCP23008 *ioExpander = new Adafruit_MCP23008();
    ioExpander->begin();
    for (int i = 0; i <= 7; i++) {
        ioExpander->pinMode(i, INPUT);
        if (active == LOW) ioExpander->pullUp(i, HIGH);
    }
    this->ioExpander = ioExpander;

    if (interrupt >= 0) {
        pinMode(interrupt, INPUT);
    }
    this->activeState = active;
    this->buttonInterrupt = interrupt;
    // todo: implement an interrupt handler for when buttons are pressed.
    // also add interrupt feature to https://github.com/adafruit/Adafruit-MCP23008-library

    return true;
}

/**
 @brief Configures the Babel language expansion chip. You must call this after
        configuring the display, as Babel needs a reference to it to function.
 @param bcs Chip select for the Babel chip.
 @returns true if Babel was successfully set up.
 @note Open Book has a default pin for Babel. The e-book wing has a solder
       jumper for Babel on the "extra" pin; if your device has a usable pin
       there, we assume you've closed the jumper. Pass in a different value for
       Babel Chip Select if you've wired it to a different GPIO.
*/
bool OpenBookDevice::configureBabel(int8_t bcs, SPIClass *spi) {
    if (this->display == NULL || bcs < 0) return false;

    BabelTypesetterGFX *typesetter = new BabelTypesetterGFX(this->display, bcs, spi);
    this->typesetter = typesetter;

    return true;
}

bool OpenBookDevice::configureSD(int8_t sdcs, SPIClass *spi) {
    this->sd = new SdFat(spi);
    this->sdcs = sdcs;

    return true;
}

bool OpenBookDevice::startDisplay() {
    this->display->begin();
    this->display->clearBuffer();
    this->display->setColorBuffer(1, false);

    return true;
}

bool OpenBookDevice::startSD() {
    return this->sd->begin(this->sdcs);
}

bool OpenBookDevice::startBabel() {
    this->typesetter->begin();

    // TODO: Babel should return a boolean to indicate success or failure.
    return true;
}

/**
 @brief Locks the device and enters a low power mode. Exit using the LOCK button.
 @details You may want to call this in response to a long timeout, or the user
          pressing the Lock button themselves. Note that this does not change
          what's on the screen; it's up to you to display an image or message
          indicating the correct way to leave this locked state.
*/
void OpenBookDevice::lockDevice() {
#ifdef ARDUINO_ARCH_RP2040
    sleep_run_from_rosc();
    sleep_goto_dormant_until_pin(12, true, false);
    // reset the device
    (*((volatile uint32_t*)(PPB_BASE + 0x0ED0C))) = 0x5FA0004;
#endif
}

double OpenBookDevice::getSystemVoltage() {
#ifdef ARDUINO_ARCH_RP2040
    analogReadResolution(16);
    pinMode(29, INPUT);
    int32_t value = analogRead(A3);
    return 3.3 * 3.3333333333 * value / 65535;
#endif
    return 0;
}

/**
 @brief Reads button state and returns it as a byte.
 @returns a bitmask with 1 for every button that is pressed, and 0 for every
          button that is not.
 @note Don't worry whether the buttons are active low or active high; this
       method abstracts that away. 1 is pressed, 0 is not pressed.
*/
uint8_t OpenBookDevice::readButtons() {
    uint8_t buttonState = 0;
    if(this->ioExpander != NULL) {
        // read from I2C
        buttonState = this->ioExpander->readGPIO();
    } else {
        if ((this->buttonConfig.select_pin != -1) && digitalRead(this->buttonConfig.select_pin)) buttonState |= OPENBOOK_BUTTONMASK_SELECT;
        if ((this->buttonConfig.next_pin != -1) && digitalRead(this->buttonConfig.next_pin)) buttonState |= OPENBOOK_BUTTONMASK_NEXT;
        if ((this->buttonConfig.previous_pin != -1) && digitalRead(this->buttonConfig.previous_pin)) buttonState |= OPENBOOK_BUTTONMASK_PREVIOUS;
        if ((this->buttonConfig.left_pin != -1) && digitalRead(this->buttonConfig.left_pin)) buttonState |= OPENBOOK_BUTTONMASK_LEFT;
        if ((this->buttonConfig.right_pin != -1) && digitalRead(this->buttonConfig.right_pin)) buttonState |= OPENBOOK_BUTTONMASK_RIGHT;
        if ((this->buttonConfig.up_pin != -1) && digitalRead(this->buttonConfig.up_pin)) buttonState |= OPENBOOK_BUTTONMASK_UP;
        if ((this->buttonConfig.down_pin != -1) && digitalRead(this->buttonConfig.down_pin)) buttonState |= OPENBOOK_BUTTONMASK_DOWN;
        if ((this->buttonConfig.lock_pin != -1) && digitalRead(this->buttonConfig.lock_pin)) buttonState |= OPENBOOK_BUTTONMASK_LOCK;
    }
    if (this->activeState == LOW) return ~buttonState; // low buttons are pressed, high buttons are being pulled up.
    else return buttonState; // high buttons are pressed, low buttons are being pulled down.
}

/**
 @brief Lets you know whether there is an SD card inserted.
 @returns one of the following three values:
          * OPEN_BOOK_SD_CARD_UNKNOWN if the device does not have a card detect signal
          * OPEN_BOOK_SD_CARD_PRESENT if the signal is available and a card is inserted
          * OPEN_BOOK_SD_CARD_NOT_PRESENT if the signal is available and no card is inserted
 @note This is designed to work with the SD card slot available on the Adafruit website.
       The one in the DigiKey 1-click BOM does not have card detect functionality; if you
       use that one, this function will always return the same value.
*/
OpenBookSDCardState OpenBookDevice::sdCardState() {
    #if defined(ODDLY_SPECIFIC_OPEN_BOOK)
    if ((this->readButtonRegister() & OPENBOOK_BUTTONMASK_SDCD) == 0) return OPEN_BOOK_SD_CARD_NOT_PRESENT;
    return OPEN_BOOK_SD_CARD_PRESENT;
    #endif

    return OPEN_BOOK_SD_CARD_UNKNOWN;
}

/**
 @returns a reference to the e-paper display, or NULL if not configured.
*/
OpenBook_IL0398 * OpenBookDevice::getDisplay() {
    return this->display;
}

/**
 @returns a reference to the Babel typesetter, or NULL if not configured.
*/
BabelTypesetterGFX * OpenBookDevice::getTypesetter() {
    return this->typesetter;
}

bool OpenBookDevice::fileExists(const char *path) {
    return this->sd->exists(path);
}

File OpenBookDevice::openFile(const char *path, oflag_t oflag) {
    return this->sd->open(path, oflag);
}
