#ifndef _THINKINK_154_TRICOLOR_Z17_H
#define _THINKINK_154_TRICOLOR_Z17_H

#include "Adafruit_EPD.h"

class ThinkInk_154_Tricolor_Z17 : public Adafruit_IL0373 {
private:
public:
  ThinkInk_154_Tricolor_Z17(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST,
                            int8_t CS, int8_t SRCS, int8_t MISO,
                            int8_t BUSY = -1)
      : Adafruit_IL0373(152, 152, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY){};

  ThinkInk_154_Tricolor_Z17(int8_t DC, int8_t RST, int8_t CS, int8_t SRCS,
                            int8_t BUSY = -1, SPIClass *spi = &SPI)
      : Adafruit_IL0373(152, 152, DC, RST, CS, SRCS, BUSY, spi){};

  void begin(thinkinkmode_t mode = THINKINK_TRICOLOR) {
    Adafruit_EPD::begin(true);
    setColorBuffer(0, true); // layer 0 uninverted
    setBlackBuffer(1, true); // layer 1 uninverted

    layer_colors[EPD_WHITE] = 0b00;
    layer_colors[EPD_BLACK] = 0b10;
    layer_colors[EPD_RED] = 0b01;
    layer_colors[EPD_GRAY] = 0b01;
    layer_colors[EPD_LIGHT] = 0b00;
    layer_colors[EPD_DARK] = 0b10;

    default_refresh_delay = 16000;

    setRotation(3);
    powerDown();
  };
};

#endif // _THINKINK_154_TRICOLOR_Z17_H
