/***************************************************
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_ThinkInk.h"

#define EPD_CS      9
#define EPD_DC      10
#define SRAM_CS     6
#define EPD_RESET   8 // can set to -1 and share with microcontroller Reset!
#define EPD_BUSY    7 // can set to -1 to not use a pin (will wait a fixed delay)

//ThinkInk_290_Grayscale4_T5 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);
ThinkInk_154_Mono_D67 display(EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY);

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  Serial.println("Adafruit counter");
  display.begin(THINKINK_MONO);
  display.setRotation(0);
}

uint16_t counter = 0;

void loop() {
  display.clearBuffer();
  display.setTextSize(4);
  display.setTextColor(EPD_BLACK);
  display.setCursor(32, 32);
  display.print((counter / 1000) % 10);
  display.print((counter / 100) % 10);
  display.print((counter / 10) % 10);
  display.print(counter % 10);

  if ((counter % 10) == 0) {
    display.display(false);
  } else {
    // redraw only 4th digit
    display.displayPartial(32+(24*3), 32, 32+(24*4), 32+(4*8));
  }

  counter++;
/*
  display.fillRect(0, 0, 16, 32, EPD_BLACK);
  display.fillRect(4, 4, 8, 24, EPD_WHITE);
 //  display.display();
  display.displayPartial(0, 0, 16, 32);
  delay(3000);
  */
}
