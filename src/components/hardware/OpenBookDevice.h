#ifndef OpenBookDevice_h
#define OpenBookDevice_h

#include <stdint.h>
#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>

#include "BabelTypesetterGFX.h"
#include "OpenBook_IL0398.h"
#include "Adafruit_MCP23008.h"

#define OPENBOOK_BUTTONMASK_LEFT (1)
#define OPENBOOK_BUTTONMASK_DOWN (2)
#define OPENBOOK_BUTTONMASK_UP (4)
#define OPENBOOK_BUTTONMASK_RIGHT (8)
#define OPENBOOK_BUTTONMASK_SELECT (16)
#define OPENBOOK_BUTTONMASK_PREVIOUS (32)
#define OPENBOOK_BUTTONMASK_NEXT (64)
#define OPENBOOK_BUTTONMASK_LOCK (128)

#ifdef ARDUINO_ARCH_RP2040
extern MbedSPI* SPI0;
extern MbedSPI* SPI1;
#endif

typedef enum {
    OPEN_BOOK_SD_CARD_PRESENT,
    OPEN_BOOK_SD_CARD_NOT_PRESENT,
    OPEN_BOOK_SD_CARD_UNKNOWN,
} OpenBookSDCardState;

typedef struct {
    int16_t left_pin;
    int16_t down_pin;
    int16_t up_pin;
    int16_t right_pin;
    int16_t select_pin;
    int16_t previous_pin;
    int16_t next_pin;
    int16_t cd_pin;
    int16_t lock_pin;
} OpenBookButtonConfig;

class OpenBookDevice {
public:
    static OpenBookDevice *sharedDevice() {
        static OpenBookDevice instance;
        return &instance;
    }
    OpenBookDevice(OpenBookDevice const&) = delete;
    void operator=(OpenBookDevice const&) = delete;

    bool startDisplay();
    bool startSD();
    bool startBabel();

    void lockDevice();

    void reset();

    double getSystemVoltage();

    uint8_t readButtons();
    OpenBookSDCardState sdCardState();
    OpenBook_IL0398 *getDisplay();
    BabelTypesetterGFX *getTypesetter();

    bool fileExists(const char *path);
    File openFile(const char *path, oflag_t oflag = FILE_READ);
    bool renameFile(const char *oldPath, const char *newPath);
    bool removeFile(const char *path);

protected:
    bool configureScreen(int8_t srcs, int8_t ecs, int8_t edc, int8_t erst, int8_t ebsy, SPIClass *spi, int width, int height);
    bool configureButtons(int8_t active, OpenBookButtonConfig config);
    bool configureI2CButtons(int8_t active, int8_t interrupt);
    bool configureBabel(int8_t bcs, SPIClass *spi);
    bool configureSD(int8_t sdcs, SPIClass *spi);

    OpenBook_IL0398 *display = NULL;
    BabelTypesetterGFX *typesetter = NULL;
    SdFat *sd;

    Adafruit_MCP23008 *ioExpander = NULL;
    OpenBookButtonConfig buttonConfig = {};
    int8_t activeState, buttonInterrupt, sdcs;
private:
    OpenBookDevice();
};

#endif // OpenBookDevice_h
