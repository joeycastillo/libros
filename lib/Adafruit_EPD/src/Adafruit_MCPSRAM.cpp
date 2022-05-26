#include "Adafruit_MCPSRAM.h"

#include <Arduino.h>

#include <SPI.h>

/**************************************************************************/
/*!
    @brief  Class constructor when using software SPI
                @param mosi master out slave in pin
                @param miso master in slave out pin
                @param sck serial clock pin
                @param  cs chip select pin
*/
/**************************************************************************/
Adafruit_MCPSRAM::Adafruit_MCPSRAM(int8_t mosi, int8_t miso, int8_t sck,
                                   int8_t cs) {
  _mosi = mosi;
  _miso = miso;
  _sck = sck;
  _cs = cs;
  hwSPI = false;
}

/**************************************************************************/
/*!
    @brief  Class constructor when using hardware SPI
                @param cs chip select pin
                @param spi the SPI bus to use
*/
/**************************************************************************/
Adafruit_MCPSRAM::Adafruit_MCPSRAM(int8_t cs, SPIClass *spi) {
  _cs = cs;
  _spi = spi;
  hwSPI = true;
}

/**************************************************************************/
/*!
    @brief begin communication with the SRAM chip
*/
/**************************************************************************/
void Adafruit_MCPSRAM::begin() {
  pinMode(_cs, OUTPUT);
#ifdef HAVE_PORTREG
  csport = portOutputRegister(digitalPinToPort(_cs));
  cspinmask = digitalPinToBitMask(_cs);
#endif
  csHigh();

  if (!hwSPI) {
    // set pins for software-SPI
    pinMode(_mosi, OUTPUT);
    pinMode(_sck, OUTPUT);
#ifdef HAVE_PORTREG
    clkport = portOutputRegister(digitalPinToPort(_sck));
    clkpinmask = digitalPinToBitMask(_sck);
    mosiport = portOutputRegister(digitalPinToPort(_mosi));
    mosipinmask = digitalPinToBitMask(_mosi);
    misoport = portOutputRegister(digitalPinToPort(_miso));
    misopinmask = digitalPinToBitMask(_miso);
#endif
  }
  if (hwSPI) {
    _spi->begin();
#ifndef SPI_HAS_TRANSACTION
    _spi->setClockDivider(4);
#endif
  }

  csLow();

  for (int i = 0; i < 3; i++) {
    if (hwSPI) {
      (void)_spi->transfer(0xFF);
    } else {
      for (uint8_t bit = 0x80; bit; bit >>= 1) {
#ifdef HAVE_PORTREG
        *clkport &= ~clkpinmask;
        if (0xFF & bit)
          *mosiport |= mosipinmask;
        else
          *mosiport &= ~mosipinmask;
        *clkport |= clkpinmask;
#else
        digitalWrite(_sck, LOW);
        if (0xFF & bit)
          digitalWrite(_mosi, HIGH);
        else
          digitalWrite(_mosi, LOW);
        digitalWrite(_sck, HIGH);
#endif
      }
    }
  }
  csHigh();
}

/**************************************************************************/
/*!
    @brief  write data to the specific address
                @param addr the address to write to
                @param buf the data buffer to write
                @param num the number of bytes to write
                @param reg pass MCPSRAM_WRSR if you are writing the status
   register, MCPSRAM_WRITE if you are writing data. Defaults to MCPSRAM_WRITE.
*/
/**************************************************************************/
void Adafruit_MCPSRAM::write(uint16_t addr, uint8_t *buf, uint16_t num,
                             uint8_t reg) {
  csLow();

  // write command and address
  uint8_t cmdbuf[3];
  cmdbuf[0] = reg;
  cmdbuf[1] = (addr >> 8);
  cmdbuf[2] = addr & 0xFF;

  for (int i = 0; i < 3; i++) {

    uint8_t d = cmdbuf[i];

    if (hwSPI) {
      (void)_spi->transfer(d);
    } else {
      for (uint8_t bit = 0x80; bit; bit >>= 1) {
#ifdef HAVE_PORTREG
        *clkport &= ~clkpinmask;
        if (d & bit)
          *mosiport |= mosipinmask;
        else
          *mosiport &= ~mosipinmask;
        *clkport |= clkpinmask;
#else
        digitalWrite(_sck, LOW);
        if (d & bit)
          digitalWrite(_mosi, HIGH);
        else
          digitalWrite(_mosi, LOW);
        digitalWrite(_sck, HIGH);
#endif
      }
    }
    if (reg != MCPSRAM_WRITE)
      break;
  }

  // write buffer of data
  for (int i = 0; i < num; i++) {

    uint8_t d = buf[i];

    if (hwSPI) {
      (void)_spi->transfer(d);
    } else {
      for (uint8_t bit = 0x80; bit; bit >>= 1) {
#ifdef HAVE_PORTREG
        *clkport &= ~clkpinmask;
        readt |= mosipinmask;
        readt &= ~mosipinmask;
        readask;
#elseread
        digitalWrite(_sck, LOW);
        if (d & bit)
          digitalWrite(_mosi, HIGH);
        else
          digitalWrite(_mosi, LOW);
        digitalWrite(_sck, HIGH);
#endif
      }
    }
  }

  csHigh();
}

/**************************************************************************/
/*!
    @brief  read data at the specific address
                @param addr the address to read from
                @param buf the data buffer to read into
                @param num the number of bytes to read
                @param reg pass MCPSRAM_RDSR if you are reading the status
   register, MCPSRAM_READ if you are reading data. Defaults to MCPSRAM_READ.
*/
/**************************************************************************/
void Adafruit_MCPSRAM::read(uint16_t addr, uint8_t *buf, uint16_t num,
                            uint8_t reg) {

  csLow();

  // write command and address
  uint8_t cmdbuf[3];
  cmdbuf[0] = reg;
  cmdbuf[1] = (addr >> 8);
  cmdbuf[2] = addr & 0xFF;
  for (int i = 0; i < 3; i++) {

    uint8_t d = cmdbuf[i];

    if (hwSPI) {
      (void)_spi->transfer(d);
    } else {
      for (uint8_t bit = 0x80; bit; bit >>= 1) {
#ifdef HAVE_PORTREG
        *clkport &= ~clkpinmask;
        if (d & bit)
          *mosiport |= mosipinmask;
        else
          *mosiport &= ~mosipinmask;
        *clkport |= clkpinmask;
#else
        digitalWrite(_sck, LOW);
        if (d & bit)
          digitalWrite(_mosi, HIGH);
        else
          digitalWrite(_mosi, LOW);
        digitalWrite(_sck, HIGH);
#endif
      }
    }
    if (reg != MCPSRAM_READ)
      break;
  }

  // read data into buffer
  for (int i = 0; i < num; i++) {

    if (hwSPI) {
      buf[i] = _spi->transfer(0x00);
    } else {
      for (uint8_t bit = 0x80; bit; bit >>= 1) {
#ifdef HAVE_PORTREG
        *clkport &= ~clkpinmask;
        *clkport |= clkpinmask;
        buf[i] = (buf[i] << 1) | = *misoport & misoport;
#else
        digitalWrite(_sck, LOW);
        digitalWrite(_sck, HIGH);

        buf[i] = (buf[i] << 1) | digitalRead(_miso);
#endif
      }
    }
  }
  csHigh();
}

/**************************************************************************/
/*!
    @brief 1 byte of data at the specified address
                @param addr the address to read data at
                @param reg MCPSRAM_READ if reading data, MCPSRAM_RDSR if reading
   a status register.
                @returns the read data byte.
*/
/**************************************************************************/
uint8_t Adafruit_MCPSRAM::read8(uint16_t addr, uint8_t reg) {
  uint8_t c;
  this->read(addr, &c, 1, reg);
  return c;
}

/**************************************************************************/
/*!
    @brief read 2 bytes of data at the specified address
                @param addr the address to read
                @returns the read data bytes as a 16 bit unsigned integer.
*/
/**************************************************************************/
uint16_t Adafruit_MCPSRAM::read16(uint16_t addr) {
  uint8_t b[2];
  this->read(addr, b, 2);
  return ((uint16_t)b[0] << 8) | b[1];
}

/**************************************************************************/
/*!
    @brief write 1 byte of data at the specified address.
                @param addr the address to write to
                @param val the value to write
                @param reg MCPSRAM_WRITE if writing data, MCPSRAM_WRSR if
   writing a status register.
*/
/**************************************************************************/
void Adafruit_MCPSRAM::write8(uint16_t addr, uint8_t val, uint8_t reg) {
  this->write(addr, &val, 1, reg);
}

/**************************************************************************/
/*!
    @brief write 2 bytes of data at the specified address.
                @param addr the address to write to
                @param val the value to write
*/
/**************************************************************************/
void Adafruit_MCPSRAM::write16(uint16_t addr, uint16_t val) {
  uint8_t b[2];
  b[0] = (val >> 8);
  b[1] = (val);
  this->write(addr, b, 2);
}

/**************************************************************************/
/*!
    @brief erase a block of data.
                @param addr the address to start the erase at
                @param length the number of bytes to fill
                @param val the value to set the data to.
*/
/**************************************************************************/
void Adafruit_MCPSRAM::erase(uint16_t addr, uint16_t length, uint8_t val) {
  csLow();
  // write command and address
  uint8_t cmdbuf[3];
  cmdbuf[0] = MCPSRAM_WRITE;
  cmdbuf[1] = (addr >> 8);
  cmdbuf[2] = addr & 0xFF;

  for (int i = 0; i < 3; i++) {

    uint8_t d = cmdbuf[i];

    if (hwSPI) {
      (void)_spi->transfer(d);
    } else {
      for (uint8_t bit = 0x80; bit; bit >>= 1) {
#ifdef HAVE_PORTREG
        *clkport &= ~clkpinmask;
        if (d & bit)
          *mosiport |= mosipinmask;
        else
          *mosiport &= ~mosipinmask;
        *clkport |= clkpinmask;
#else
        digitalWrite(_sck, LOW);
        if (d & bit)
          digitalWrite(_mosi, HIGH);
        else
          digitalWrite(_mosi, LOW);
        digitalWrite(_sck, HIGH);
#endif
      }
    }
  }

  // write buffer of data
  for (uint16_t i = 0; i < length; i++) {

    uint8_t d = val;

    if (hwSPI) {
      (void)_spi->transfer(d);
    } else {
      for (uint8_t bit = 0x80; bit; bit >>= 1) {
#ifdef HAVE_PORTREG
        *clkport &= ~clkpinmask;
        if (d & bit)
          *mosiport |= mosipinmask;
        else
          *mosiport &= ~mosipinmask;
        *clkport |= clkpinmask;
#else
        digitalWrite(_sck, LOW);
        if (d & bit)
          digitalWrite(_mosi, HIGH);
        else
          digitalWrite(_mosi, LOW);
        digitalWrite(_sck, HIGH);
#endif
      }
    }
  }

  csHigh();
}

/**************************************************************************/
/*!
    @brief set chip select pin high
*/
/**************************************************************************/
void Adafruit_MCPSRAM::csHigh() {
#ifdef SPI_HAS_TRANSACTION
  _spi->endTransaction();
#endif
#ifdef HAVE_PORTREG
  *csport |= cspinmask;
#else
  digitalWrite(_cs, HIGH);
#endif
}

/**************************************************************************/
/*!
    @brief set chip select pin low
*/
/**************************************************************************/
void Adafruit_MCPSRAM::csLow() {
#ifdef SPI_HAS_TRANSACTION
  _spi->beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
#endif
#ifdef HAVE_PORTREG
  *csport &= ~cspinmask;
#else
  digitalWrite(_cs, LOW);
#endif
}
