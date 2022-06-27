/*
 * The MIT License (MIT)
 *
 * Copyright © 2019 Joey Castillo. All rights reserved.
 * Incorporates ideas and code from the Adafruit_GFX library.
 * Copyright (c) 2013 Adafruit Industries.  All rights reserved.
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

#include <string.h>
#include "BabelTypesetterGFX.h"

BabelTypesetterGFX::BabelTypesetterGFX(Adafruit_GFX *display, uint8_t cs, SPIClass *spi) {
    this->display = display;
    this->babelDevice = new BabelSPIFlash(cs, spi);
}

#if BOARD_REQUIRES_BABEL_FILE
BabelTypesetterGFX::BabelTypesetterGFX(Adafruit_GFX *display, FatFileSystem *fatfs, char *filename) {
    this->display = display;
    this->babelDevice = new BabelFile(fatfs, filename);
}
#endif

bool BabelTypesetterGFX::begin() {
    return BabelTypesetter::begin();
}

void BabelTypesetterGFX::drawPixel(int16_t x, int16_t y, uint16_t color) {
    this->display->writePixel(x, y, color);
}

void BabelTypesetterGFX::drawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    this->display->writeFillRect(x, y, w, h, color);
}
