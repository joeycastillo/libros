/*!
 * @file Adafruit_IL0398.cpp
 *
 * Forked from Adafruit_IL0398.cpp; copyright notce below.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Dean Miller for Adafruit Industries.
 *
 * BSD license, all text here must be included in any redistribution.
 *
 */

#include "OpenBook_SSD1683.h"

#define BUSY_WAIT 500

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif
/**************************************************************************/
/*!
    @brief constructor if using external SRAM chip and software SPI
    @param width the width of the display in pixels
    @param height the height of the display in pixels
    @param SID the SID pin to use
    @param SCLK the SCLK pin to use
    @param DC the data/command pin to use
    @param RST the reset pin to use
    @param CS the chip select pin to use
    @param SRCS the SRAM chip select pin to use
    @param MISO the MISO pin to use
    @param BUSY the busy pin to use
*/
/**************************************************************************/
OpenBook_SSD1683::OpenBook_SSD1683(int width, int height, 
				  int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, 
				  int8_t CS, int8_t SRCS, int8_t MISO, int8_t BUSY) : 
  Adafruit_EPD(width, height, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY) {

  buffer1_size = ((uint32_t)width * (uint32_t)height) / 8;
  buffer2_size = buffer1_size;

  if (SRCS >= 0) {
    use_sram = true;
    buffer1_addr = 0;
    buffer2_addr = buffer1_size;
    buffer1 = buffer2 = NULL;
  } else {
    buffer1 = (uint8_t *)malloc(buffer1_size);
    buffer2 = (uint8_t *)malloc(buffer2_size);
  }
}

// constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset
/**************************************************************************/
/*!
    @brief constructor if using on-chip RAM and hardware SPI
    @param width the width of the display in pixels
    @param height the height of the display in pixels
    @param DC the data/command pin to use
    @param RST the reset pin to use
    @param CS the chip select pin to use
    @param SRCS the SRAM chip select pin to use
    @param BUSY the busy pin to use
*/
/**************************************************************************/
OpenBook_SSD1683::OpenBook_SSD1683(int width, int height, 
				  int8_t DC, int8_t RST, 
				  int8_t CS, int8_t SRCS, int8_t BUSY, SPIClass *spi) :
  Adafruit_EPD(width, height, DC, RST, CS, SRCS, BUSY, spi) {

  buffer1_size = ((uint32_t)width * (uint32_t)height) / 8;
  buffer2_size = buffer1_size;

  if (SRCS >= 0) {
    use_sram = true;
    buffer1_addr = 0;
    buffer2_addr = 0;
    buffer1 = buffer2 = NULL;
  } else {
    buffer1 = (uint8_t *)malloc(buffer1_size);
    buffer2 = (uint8_t *)malloc(buffer1_size);
  }
}

/**************************************************************************/
/*!
    @brief wait for busy signal to end
*/
/**************************************************************************/
void OpenBook_SSD1683::busy_wait(void)
{
  if (_busy_pin > -1) {
    do {
    } while (digitalRead(_busy_pin)); //wait for busy low
  } else {
    delay(BUSY_WAIT);
  }

}

/**************************************************************************/
/*!
    @brief begin communication with and set up the display.
    @param reset if true the reset pin will be toggled.
*/
/**************************************************************************/
void OpenBook_SSD1683::begin(bool reset)
{
  Adafruit_EPD::begin(reset);
  setBlackBuffer(0, true);
  powerDown();
}

/**************************************************************************/
/*!
    @brief signal the display to update
*/
/**************************************************************************/
void OpenBook_SSD1683::update()
{
  uint8_t buf[5];

  switch (this->currentDisplayMode) {
    case OPEN_BOOK_DISPLAY_MODE_QUICK:
      buf[0] = 0xC7;
      break;
    case OPEN_BOOK_DISPLAY_MODE_PARTIAL:
    case OPEN_BOOK_DISPLAY_MODE_FASTPARTIAL:
    case OPEN_BOOK_DISPLAY_MODE_GRAYSCALE:
    case OPEN_BOOK_DISPLAY_MODE_DEFAULT:
    default:
      buf[0] = 0xf7;
      break;
  }

  EPD_command(0x22, buf, 1);

  EPD_command(0x20);

  busy_wait();
  if (_busy_pin <= -1) {
    delay(5000);
  }
}


/**************************************************************************/
/*!
    @todo Custom lookup tables for full screen updates.
*/
/**************************************************************************/

const unsigned char OpenBook_SSD1683::LUT_W[] PROGMEM =
{
};
const unsigned char OpenBook_SSD1683::LUT_B[] PROGMEM =
{
};
const unsigned char OpenBook_SSD1683::LUT_VCOM_FULL[] PROGMEM =
{
};

/**************************************************************************/
/*!
    @todo Custom lookup tables for partial screen updates.
*/
/**************************************************************************/

const unsigned char OpenBook_SSD1683::LUT_WW_PARTIAL[] PROGMEM =
{
};

const unsigned char OpenBook_SSD1683::LUT_WB_PARTIAL[] PROGMEM =
{
};

const unsigned char OpenBook_SSD1683::LUT_BW_PARTIAL[] PROGMEM =
{
};

const unsigned char OpenBook_SSD1683::LUT_BB_PARTIAL[] PROGMEM =
{
};

const unsigned char OpenBook_SSD1683::LUT_VCOM_PARTIAL[] PROGMEM =
{
};

/**************************************************************************/
/*!
    @todo Lookup tables for fast partial update
*/
/**************************************************************************/

const unsigned char OpenBook_SSD1683::LUT_WW_FASTPARTIAL[] PROGMEM =
{
};

const unsigned char OpenBook_SSD1683::LUT_WB_FASTPARTIAL[] PROGMEM =
{
};

const unsigned char OpenBook_SSD1683::LUT_BW_FASTPARTIAL[] PROGMEM =
{
};

const unsigned char OpenBook_SSD1683::LUT_BB_FASTPARTIAL[] PROGMEM =
{
};

const unsigned char OpenBook_SSD1683::LUT_VCOM_FASTPARTIAL[] PROGMEM =
{
};

/**************************************************************************/
/*!
    @todo Lookup tables for 4-color grayscale.
*/
/**************************************************************************/

const unsigned char OpenBook_SSD1683::LUT_VCOM_GRAYSCALE[] PROGMEM = 
{
};

const unsigned char OpenBook_SSD1683::LUT_WW_GRAYSCALE[] PROGMEM = 
{
};

const unsigned char OpenBook_SSD1683::LUT_WB_GRAYSCALE[] PROGMEM = 
{
};

const unsigned char OpenBook_SSD1683::LUT_BW_GRAYSCALE[] PROGMEM = 
{
};

const unsigned char OpenBook_SSD1683::LUT_BB_GRAYSCALE[] PROGMEM = 
{
};

/**************************************************************************/
/*!
    @brief start up the display. Same as init, but here for compatibility with
           Adafruit_EPD; you can call OpenBook_SSD1683::init with more options.
*/
/**************************************************************************/
void OpenBook_SSD1683::powerUp() {
    this->init(this->currentDisplayMode);
}

/**************************************************************************/
/*!
    @brief start up the display
    @param displayMode an enum indicating which refresh waveform should be loaded in. Default is to use the factory-supplied waveform.
    @warning You always need to do one full refresh before enabling partial mode.
*/
/**************************************************************************/
void OpenBook_SSD1683::init(OpenBookDisplayMode displayMode) {
  uint8_t buf[5];

  hardwareReset();

  EPD_command(0x12); // SWRESET

  busy_wait();

  buf[0] = 0x40;
  buf[1] = 0x00;
  EPD_command(0x21, buf, 2);

  buf[0] = 0x05;
  EPD_command(0x3C, buf, 1); // Set border waveform

  buf[0] = 0x2B;
  buf[1] = 0x01;
  buf[2] = 0x00;
  EPD_command(0x01, buf, 3); // Set MUX as 300

  buf[0] = 0x03;
  EPD_command(0x11, buf, 1); // set ram entry mode to normal

  buf[0] = 0x00;
  buf[1] = 0x31;
  EPD_command(0x44, buf, 2); // Set ram X address

  buf[0] = 0x00;
  buf[1] = 0x00;
  buf[2] = 0x2b;
  buf[3] = 0x01;
  EPD_command(0x45, buf, 4); // Set ram Y address


  this->setRAMAddress(0, 0);

  switch (displayMode) {
    case OPEN_BOOK_DISPLAY_MODE_QUICK:
        buf[0] = 0x5a;
        EPD_command(0x1A, buf, 1); // Write to temperature register
        buf[0] = 0x91;
        EPD_command(0x22, buf, 1);
        EPD_command(0x20);
        busy_wait();
        break;
    case OPEN_BOOK_DISPLAY_MODE_PARTIAL:
        // TODO: Implement partial refresh mode
        break;
    case OPEN_BOOK_DISPLAY_MODE_FASTPARTIAL:
        // TODO: Implement fast partial refresh mode
        break;
    case OPEN_BOOK_DISPLAY_MODE_GRAYSCALE:
        // TODO: Implement grayscale mode
        break;
    case OPEN_BOOK_DISPLAY_MODE_DEFAULT:
        // Nothing to do here!
        break;
    default:
        break;
  }

  this->currentDisplayMode = displayMode;
}

/**************************************************************************/
/*!
    @brief wind down the display
*/
/**************************************************************************/
void OpenBook_SSD1683::powerDown()
{
  // uint8_t data = 0x01;
  // EPD_command(0x10, &data, 1); // power off
}

/**************************************************************************/
/*!
    @todo Sets the window for partial refresh.
*/
/**************************************************************************/
void OpenBook_SSD1683::setWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
}

/**************************************************************************/
/*!
    @brief Sets the display waveforms for a particular display mode
    @param displayMode the desired mode

*/
/**************************************************************************/
void OpenBook_SSD1683::setDisplayMode(OpenBookDisplayMode displayMode) {
    this->currentDisplayMode = displayMode;
}

/**************************************************************************/
/*!
    @brief draw a single pixel on the screen
	@param x the x axis position
	@param y the y axis position
	@param color the color of the pixel
*/
/**************************************************************************/
void OpenBook_SSD1683::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
  return;
      
  uint8_t *pByte1;
  uint8_t *pByte2;

  // check rotation, move pixel around if necessary
  switch (getRotation()) {
    case 1:
    EPD_swap(x, y);
    x = WIDTH - x - 1;
    break;
    case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    break;
    case 3:
    EPD_swap(x, y);
    y = HEIGHT - y - 1;
    break;
  }
      
  uint16_t addr = ( (uint32_t)(WIDTH - 1 - x) * (uint32_t)HEIGHT + y)/8;

  if (use_sram) {
    uint8_t byte1 = sram.read8(blackbuffer_addr + addr);
    uint8_t byte2 = sram.read8(colorbuffer_addr + addr);
    pByte1 = &byte1;
    pByte2 = &byte2;
  } else {
    pByte1 = black_buffer + addr;
    pByte2 = color_buffer + addr;
  }
  
  switch (color) {
    case EPD_BLACK:   
      *pByte1 &= ~(1 << (7 - (y%8)));
      *pByte2 &= ~(1 << (7 - (y%8)));
      break;
    case EPD_DARK:
        *pByte1 |= (1 << (7 - (y%8)));
        *pByte2 &= ~(1 << (7 - (y%8)));
        break;
    case EPD_LIGHT:
        *pByte1 &= ~(1 << (7 - (y%8)));
        *pByte2 |= (1 << (7 - (y%8)));
        break;
    case EPD_WHITE:
        *pByte1 |= (1 << (7 - (y%8)));
        *pByte2 |= (1 << (7 - (y%8)));
        break;
  }
  
  if (use_sram) {
    sram.write8(addr, *pByte1);
    sram.write8(addr + buffer1_size, *pByte2);
  }
}

/**************************************************************************/
/*!
    @brief Updates the full screen.
*/
/**************************************************************************/
void OpenBook_SSD1683::display() {
  Adafruit_EPD::display();
  busy_wait();
}

void OpenBook_SSD1683::displayGrayscale(uint16_t x, uint16_t y, const unsigned char *bitmap, uint16_t w, uint16_t h) {
    uint16_t byteWidth = (w*2 + 7) / 8;
    uint8_t byte = 0;
    uint16_t color;

    this->startWrite();
    for(uint16_t j=0; j<h; j++, y++) {
        for(uint16_t i=0; i < w * 2; i += 2 ) {
            if(i & 7) byte <<= 2;
            else byte = bitmap[j * byteWidth + i / 8];
            switch((byte & 0xC0) >> 6) {
                case 0:
                    color = EPD_BLACK;
                    break;
                case 1:
                    color = EPD_DARK;
                    break;
                case 2:
                    color = EPD_LIGHT;
                    break;
                case 3:
                    color = EPD_WHITE;
                    break;
                default:
                    break;
            }
            this->writePixel(x+i/2, y, color);
        }
    }
    this->endWrite();
}

/**************************************************************************/
/*!
    @todo Updates a part of the screen.
    @param x the x origin of the area you want to update. May be rounded down to a multiple of 8.
    @param y the y origin of the area you want to update. May be rounded down to a multiple of 8.
    @param w the width of the area you want to update. May be rounded up to a multiple of 8.
    @param h the height of the area you want to update. May be rounded up to a multiple of 8.
    @note You can make whatever changes to the buffer you want before calling this, but
          only the area in the update rect will be updated. If, say, you fill the buffer
          with black, but then update only an 8x8 rect, the EPD will show whatever was on
          the screen last (plus your 8x8 black rect), but buf will still be full of black,
          and subsequently calling for a full display() will fill the screen with black.
          Suggest only making changes to an area that you mark dirty, and then calling
          this with the dirty rect so that the buffer and the screen stay consistent.
*/
/**************************************************************************/
void OpenBook_SSD1683::displayPartial(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
  // TODO: Implement partial refresh mode
  this->display();
}

/**************************************************************************/
/*!
    @brief Send the specific command to start writing to EPD display RAM
    @param index The index for which buffer to write (0 or 1 or tri-color displays)
    Ignored for monochrome displays.
    @returns The byte that is read from SPI at the same time as sending the command
*/
/**************************************************************************/
uint8_t OpenBook_SSD1683::writeRAMCommand(uint8_t index) {
#ifdef EPD_DEBUG
  Serial.println("  writeRAMCommand");
#endif
  if (index == 0) {
    return EPD_command(0x24, false);
  }
  if (index == 1) {
    return EPD_command(0x26, false);
  }
  return 0;
}

/**************************************************************************/
/*!
    @brief Some displays require setting the RAM address pointer
    @param x X address counter value
    @param y Y address counter value
*/
/**************************************************************************/
void OpenBook_SSD1683::setRAMAddress(uint16_t x, uint16_t y) {
  uint8_t buf[2];

  buf[0] = 0;
  EPD_command(0x4e, buf, 1); // set RAM X address counter

  buf[0] = 0;
  buf[1] = 0;
  EPD_command(0x4f, buf, 2); // set RAM Y address counter
}
