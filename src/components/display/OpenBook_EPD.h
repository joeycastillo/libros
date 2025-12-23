#ifndef OpenBook_EPD_H
#define OpenBook_EPD_H

#include "Adafruit_EPD.h"

typedef enum OpenBookDisplayMode {
    OPEN_BOOK_DISPLAY_MODE_DEFAULT,
    OPEN_BOOK_DISPLAY_MODE_QUICK,
    OPEN_BOOK_DISPLAY_MODE_PARTIAL,
    OPEN_BOOK_DISPLAY_MODE_FASTPARTIAL,
    OPEN_BOOK_DISPLAY_MODE_GRAYSCALE
}
OpenBookDisplayMode;

/**************************************************************************/
/*!
    @brief  Abstract base class for OpenBook EPD drivers, providing common
            interface for display mode management and OpenBook-specific features.
*/
/**************************************************************************/
class OpenBook_EPD : public Adafruit_EPD {
public:
    // Constructors - must be implemented by derived classes
    OpenBook_EPD(int width, int height, int8_t SID, int8_t SCLK, int8_t DC,
                 int8_t RST, int8_t CS, int8_t SRCS, int8_t MISO, int8_t BUSY = -1)
        : Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY) {}

    OpenBook_EPD(int width, int height, int8_t DC, int8_t RST, int8_t CS,
                 int8_t SRCS, int8_t BUSY = -1, SPIClass *spi = &SPI)
        : Adafruit_EPD(width, height, DC, RST, CS, SRCS, BUSY, spi) {}

    // OpenBook-specific methods that must be implemented by derived classes
    virtual void setDisplayMode(OpenBookDisplayMode displayMode) = 0;
    virtual OpenBookDisplayMode getDisplayMode() = 0;
    virtual void displayGrayscale(uint16_t x, uint16_t y, const unsigned char *bitmap, uint16_t w, uint16_t h) = 0;
    virtual void displayPartial(uint16_t x, uint16_t y, uint16_t w, uint16_t h) = 0;
};

#endif // OpenBook_EPD_H