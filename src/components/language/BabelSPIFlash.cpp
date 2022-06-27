/*
 * The MIT License (MIT)
 *
 * Copyright © 2019 Joey Castillo. All rights reserved.
 *     Portions of this file were cribbed from the Adafruit_SPIFlash library.
 * Copyright (c) 2019 Ha Thach and Dean Miller for Adafruit Industries LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "BabelSPIFlash.h"
#include <Arduino.h>

#define GD25Q16C_MAX_CLOCK_SPEED_MHZ (60)

#define SFLASH_CMD_READ 0x03
#define SFLASH_CMD_READ_STATUS 0x05
#define SFLASH_CMD_READ_STATUS2 0x35
#define SFLASH_CMD_ENABLE_RESET 0x66
#define SFLASH_CMD_RESET 0x99
#define SFLASH_CMD_WRITE_DISABLE 0x04

BabelSPIFlash::BabelSPIFlash(uint8_t cs, SPIClass *spi) {
    this->cs = cs;
    this->spi = spi;
}

bool BabelSPIFlash::begin() {
    pinMode(this->cs, OUTPUT);
    digitalWrite(this->cs, HIGH);
    this->spi->begin();

    // We don't know what state the flash is in so wait for any remaining writes and then reset.
    // The write in progress bit should be low.
    while (readStatus() & 0x01 );
    // The suspended write/erase bit should be low.
    while (readStatus2() & 0x80 );

    this->runCommand(SFLASH_CMD_ENABLE_RESET);
    this->runCommand(SFLASH_CMD_RESET);
    this->runCommand(SFLASH_CMD_WRITE_DISABLE);

    // Wait 30us for the reset
    delayMicroseconds(30);

    // Speed up to max device frequency
    this->settings = SPISettings(GD25Q16C_MAX_CLOCK_SPEED_MHZ*1000000UL, MSBFIRST, SPI_MODE0);

    delay(50);

    return BabelDevice::begin();
}

void BabelSPIFlash::read(uint32_t addr, void *data, uint32_t len) {
    this->read(addr, (uint8_t *)data, len);
}

// The following methods were adapted from Adafruit_SPIFlash.
// We don't need to support all the Flash chips or all the features of that library,
// for our purposes it will suffice to check status, reset the chip and read data.

bool BabelSPIFlash::runCommand(uint8_t command) {
    digitalWrite(this->cs, LOW);
    this->spi->beginTransaction(this->settings);

    this->spi->transfer(command);

    this->spi->endTransaction();
    digitalWrite(this->cs, HIGH);

    return true;
}

bool BabelSPIFlash::readCommand(uint8_t command, uint8_t* response, uint32_t len) {
    digitalWrite(this->cs, LOW);
    this->spi->beginTransaction(this->settings);

    this->spi->transfer(command);
    while(len--) {
        *response++ = this->spi->transfer(0xFF);
    }

    this->spi->endTransaction();
    digitalWrite(this->cs, HIGH);

    return true;
}

uint8_t BabelSPIFlash::readStatus() {
  uint8_t status;
  this->readCommand(SFLASH_CMD_READ_STATUS, &status, 1);
  return status;
}

uint8_t BabelSPIFlash::readStatus2(void) {
  uint8_t status;
  this->readCommand(SFLASH_CMD_READ_STATUS2, &status, 1);
  return status;
}

void BabelSPIFlash::read(uint32_t addr, uint8_t *data, uint32_t len) {
  digitalWrite(this->cs, LOW);
  this->spi->beginTransaction(this->settings);

  this->spi->transfer(SFLASH_CMD_READ);

  // 24-bit address MSB first
  this->spi->transfer((addr >> 16) & 0xFF);
  this->spi->transfer((addr >> 8) & 0xFF);
  this->spi->transfer(addr & 0xFF);

  while(len--)
  {
    *data++ = this->spi->transfer(0xFF);
  }

  this->spi->endTransaction();
  digitalWrite(this->cs, HIGH);
}
