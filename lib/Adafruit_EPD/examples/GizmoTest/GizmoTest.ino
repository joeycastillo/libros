/***************************************************
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include "Adafruit_EPD.h"

#define EPD_CS      0
#define EPD_DC      1
#define SRAM_CS     -1
#define EPD_RESET   PIN_A3 // can set to -1 and share with microcontroller Reset!
#define EPD_BUSY    -1 // can set to -1 to not use a pin (will wait a fixed delay)

// You will need to use Adafruit's CircuitPlayground Express Board Definition
// for Gizmos rather than the Arduino version since there are additional SPI
// ports exposed.
#if (SPI_INTERFACES_COUNT == 1)
  SPIClass* spi = &SPI;
#else
  SPIClass* spi = &SPI1;
#endif

Adafruit_IL0373 display(152, 152, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS, EPD_BUSY, spi);

float p = 3.1415926;

void setup(void) {
  Serial.begin(9600);
  Serial.print("Hello! EPD Test");

  display.begin();
  display.setRotation(3);
  
  Serial.println("Initialized");

  display.clearBuffer();
  display.fillRect(display.width()/3, 0, display.width()/3, display.height(), EPD_RED);
  display.fillRect((display.width()*2)/3, 0, display.width()/3, display.height(), EPD_BLACK);
  display.display();

  delay(15 * 1000);

  // large block of text
  display.clearBuffer();
  display.fillScreen(EPD_WHITE);
  testdrawtext("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", EPD_BLACK);

  delay(15 * 1000);

  // epd print function!
  epdPrintTest();

  delay(15 * 1000);

  // a single pixel
  display.clearBuffer();
  display.drawPixel(display.width()/2, display.height()/2, EPD_BLACK);

  delay(15 * 1000);

  testtriangles();

  delay(15 * 1000);
  
  // line draw test
  testlines(EPD_BLACK);

  delay(15 * 1000);

  // optimized lines
  testfastlines(EPD_BLACK, EPD_RED);

  delay(15 * 1000);

  testdrawrects(EPD_RED);

  delay(15 * 1000);

  testfillrects(EPD_BLACK, EPD_RED);

  delay(15 * 1000);

  display.fillScreen(EPD_WHITE);
  testfillcircles(10, EPD_RED);
  testdrawcircles(10, EPD_BLACK);

  delay(15 * 1000);

  testroundrects();

  delay(15 * 1000);

  mediabuttons();

  Serial.println("done");
}

void loop() {
  delay(500);
}

void testlines(uint16_t color) {
  display.clearBuffer();
  display.fillScreen(EPD_WHITE);
  for (int16_t x=0; x < display.width(); x+=6) {
    display.drawLine(0, 0, x, display.height()-1, color);
  }
  for (int16_t y=0; y < display.height(); y+=6) {
    display.drawLine(0, 0, display.width()-1, y, color);
  }

  display.fillScreen(EPD_WHITE);
  for (int16_t x=0; x < display.width(); x+=6) {
    display.drawLine(display.width()-1, 0, x, display.height()-1, color);
  }
  for (int16_t y=0; y < display.height(); y+=6) {
    display.drawLine(display.width()-1, 0, 0, y, color);
  }

  display.fillScreen(EPD_WHITE);
  for (int16_t x=0; x < display.width(); x+=6) {
    display.drawLine(0, display.height()-1, x, 0, color);
  }
  for (int16_t y=0; y < display.height(); y+=6) {
    display.drawLine(0, display.height()-1, display.width()-1, y, color);
  }

  display.fillScreen(EPD_WHITE);
  for (int16_t x=0; x < display.width(); x+=6) {
    display.drawLine(display.width()-1, display.height()-1, x, 0, color);
  }
  for (int16_t y=0; y < display.height(); y+=6) {
    display.drawLine(display.width()-1, display.height()-1, 0, y, color);
  }
  display.display();
}

void testdrawtext(char *text, uint16_t color) {
  display.clearBuffer();
  display.setCursor(5, 5);
  display.setTextColor(color);
  display.setTextWrap(true);
  display.print(text);
  display.display();
}

void testfastlines(uint16_t color1, uint16_t color2) {
  display.clearBuffer();
  display.fillScreen(EPD_WHITE);
  for (int16_t y=0; y < display.height(); y+=5) {
    display.drawFastHLine(0, y, display.width(), color1);
  }
  for (int16_t x=0; x < display.width(); x+=5) {
    display.drawFastVLine(x, 0, display.height(), color2);
  }
  display.display();
}

void testdrawrects(uint16_t color) {
  display.clearBuffer();
  display.fillScreen(EPD_WHITE);
  for (int16_t x=0; x < display.width(); x+=6) {
    display.drawRect(display.width()/2 -x/2, display.height()/2 -x/2 , x, x, color);
  }
  display.display();
}

void testfillrects(uint16_t color1, uint16_t color2) {
  display.clearBuffer();
  display.fillScreen(EPD_WHITE);
  for (int16_t x=display.width()-1; x > 6; x-=6) {
    display.fillRect(display.width()/2 -x/2, display.height()/2 -x/2 , x, x, color1);
    display.drawRect(display.width()/2 -x/2, display.height()/2 -x/2 , x, x, color2);
  }
  display.display();
}

void testfillcircles(uint8_t radius, uint16_t color) {
  display.clearBuffer();
  for (int16_t x=radius; x < display.width(); x+=radius*2) {
    for (int16_t y=radius; y < display.height(); y+=radius*2) {
      display.fillCircle(x, y, radius, color);
    }
  }
  display.display();
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  display.clearBuffer();
  for (int16_t x=0; x < display.width()+radius; x+=radius*2) {
    for (int16_t y=0; y < display.height()+radius; y+=radius*2) {
      display.drawCircle(x, y, radius, color);
    }
  }
  display.display();
}

void testtriangles() {
  display.clearBuffer();
  display.fillScreen(EPD_WHITE);
  int color = EPD_BLACK;
  int t;
  int w = display.width()/2;
  int x = display.height()-1;
  int y = 0;
  int z = display.width();
  for(t = 0 ; t <= 15; t++) {
    display.drawTriangle(w, y, y, x, z, x, color);
    x-=4;
    y+=4;
    z-=4;
    if(t == 8) color = EPD_RED;
  }
  display.display();
}

void testroundrects() {
  display.clearBuffer();
  display.fillScreen(EPD_WHITE);
  int color = EPD_BLACK;
  int i;
  int t;
  for(t = 0 ; t <= 4; t+=1) {
    int x = 0;
    int y = 0;
    int w = display.width()-2;
    int h = display.height()-2;
    for(i = 0 ; i <= 16; i+=1) {
      display.drawRoundRect(x, y, w, h, 5, color);
      x+=2;
      y+=3;
      w-=4;
      h-=6;
      if(i == 7) color = EPD_RED;
    }
    color = EPD_BLACK;
  }
  display.display();
}

void epdPrintTest() {
  display.clearBuffer();
  display.setCursor(5, 5);
  display.fillScreen(EPD_WHITE);
  display.setTextColor(EPD_BLACK);
  display.setTextSize(2);
  display.println("Hello World!");
  display.setTextSize(1);
  display.setTextColor(EPD_RED);
  display.print(p, 6);
  display.println(" Want pi?");
  display.println(" ");
  display.print(8675309, HEX); // print 8,675,309 out in HEX!
  display.println(" Print HEX!");
  display.println(" ");
  display.setTextColor(EPD_BLACK);
  display.println("Sketch has been");
  display.println("running for: ");
  display.setTextColor(EPD_RED);
  display.print(millis() / 1000);
  display.setTextColor(EPD_BLACK);
  display.print(" seconds.");
  display.display();
}

void mediabuttons() {
  display.clearBuffer();
  // play
  display.fillScreen(EPD_WHITE);
  display.fillRoundRect(25, 10, 78, 60, 8, EPD_BLACK);
  display.fillTriangle(42, 20, 42, 60, 90, 40, EPD_RED);
  // pause
  display.fillRoundRect(25, 90, 78, 60, 8, EPD_BLACK);
  display.fillRoundRect(39, 98, 20, 45, 5, EPD_RED);
  display.fillRoundRect(69, 98, 20, 45, 5, EPD_RED);
  display.display();
}
