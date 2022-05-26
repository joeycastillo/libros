/*
 * The MIT License (MIT)
 *
 * Copyright © 2019 Joey Castillo. All rights reserved.
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

#ifndef BabelSPIFlash_h
#define BabelSPIFlash_h

#include <stdio.h>
#include "SPI.h"
#include "BabelDevice.h"

class BabelSPIFlash: public BabelDevice {
public:
    BabelSPIFlash(uint8_t cs, SPIClass *spi);
    void begin();
    void read(uint32_t addr, void *data, uint32_t len);
private:
    uint8_t cs;
    SPIClass *spi;
    SPISettings settings;

    void read(uint32_t addr, uint8_t *data, uint32_t len);
    bool runCommand(uint8_t command);
    bool readCommand(uint8_t command, uint8_t* response, uint32_t len);
    uint8_t readStatus();
    uint8_t readStatus2();
};

#endif /* BabelSPIFlash_h */
