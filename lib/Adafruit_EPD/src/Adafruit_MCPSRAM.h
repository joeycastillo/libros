#include <Arduino.h>
#include <SPI.h>

#define MCPSRAM_READ 0x03  ///< read command
#define MCPSRAM_WRITE 0x02 ///< write command
#define MCPSRAM_RDSR 0x05  ///< read status register command
#define MCPSRAM_WRSR 0x01  ///< write status register command

#define K640_SEQUENTIAL_MODE (1 << 6) ///< put ram chip in sequential mode

/**************************************************************************/
/*!
    @brief  Class for interfacing with Microchip SPI SRAM chips
*/
/**************************************************************************/
class Adafruit_MCPSRAM {
public:
  Adafruit_MCPSRAM(int8_t mosi, int8_t miso, int8_t sck, int8_t cs);
  Adafruit_MCPSRAM(int8_t cs, SPIClass *spi = &SPI);
  ~Adafruit_MCPSRAM() {}

  void begin();

  void write(uint16_t addr, uint8_t *buf, uint16_t num,
             uint8_t reg = MCPSRAM_WRITE);
  void read(uint16_t addr, uint8_t *buf, uint16_t num,
            uint8_t reg = MCPSRAM_READ);
  void erase(uint16_t addr, uint16_t length, uint8_t val = 0x00);

  uint8_t read8(uint16_t addr, uint8_t reg = MCPSRAM_READ);
  uint16_t read16(uint16_t addr);

  void write8(uint16_t addr, uint8_t val, uint8_t reg = MCPSRAM_WRITE);
  void write16(uint16_t addr, uint16_t val);

  void csHigh();
  void csLow();

private:
  boolean hwSPI; ///< true if using hardware SPI
#ifdef HAVE_PORTREG
  PortReg *mosiport, *clkport, *csport, *misoport;
  PortMask mosipinmask, clkpinmask, cspinmask, misopinmask;
#endif
  int8_t _cs, _mosi, _miso, _sck;
  SPIClass *_spi = NULL;
};
