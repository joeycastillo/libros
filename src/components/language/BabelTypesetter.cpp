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
#include <stdlib.h>
#include "BabelTypesetter.h"

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

BabelTypesetter::BabelTypesetter() {
}

void BabelTypesetter::begin() {
    this->babelDevice->begin();
}

void BabelTypesetter::setCursor(int16_t x, int16_t y) {
    this->cursor.x = x;
    this->cursor.y = y;
}

int16_t BabelTypesetter::getCursorX() {
    return this->cursor.x;
}

int16_t BabelTypesetter::getCursorY() {
    return this->cursor.y;
}

void BabelTypesetter::resetCursor() {
    this->setCursor(this->minX, this->minY);
}

void BabelTypesetter::setLayoutArea(int16_t x, int16_t y, int16_t w, int16_t h) {
    this->minX = x;
    this->minY = y;
    this->maxX = x + w;
    this->maxY = y + h;
    this->setCursor(x, y);
    if (this->lineWidth != 0) this->lineWidth = this->maxX - this->minX;
}

void BabelTypesetter::drawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    for (int16_t i=x; i<x+w; i++) {
        for (int16_t j=y; j<y+h; j++) {
            this->drawPixel(x, y, color);
        }
    }
}

int BabelTypesetter::drawGlyph(int16_t x, int16_t y, BabelGlyph glyph, uint16_t color, uint8_t size) {\
    int retVal = 0;

    for (uint8_t i = 0; i < (this->bold ? 2 : 1); i++) {
        if (this->italic) {
            for(uint8_t j = 0; j < 4; j++) {
                retVal = this->drawGlyph(x + i + 2 - j, y, glyph, color, size, j * 4, j * 4 + 4);
            }
        } else {
            retVal = this->drawGlyph(x + i, y, glyph, color, size, 0, this->babelDevice->getHeight());
        }
    }

    return retVal;
}

int BabelTypesetter::drawGlyph(int16_t x, int16_t y, BabelGlyph glyph, uint16_t color, uint8_t size, uint8_t startY, uint8_t endY) {
    uint8_t width = BABEL_INFO_GET_GLYPH_WIDTH(glyph.info);
    uint8_t characterWidth = width > 8 ? 2 : 1; // <=8x16 glyphs fit in 16 bytes. >8x16 require two.
    bool mirrored = ((1 == -1) && BABEL_INFO_GET_MIRRORED_IN_RTL(glyph.info));

    if (mirrored) {
        switch (characterWidth) {
            case 1:
                for(int8_t i=startY; i<(characterWidth*endY); i++ ) {
                    uint8_t line = glyph.glyphData[i];
                    for(int8_t j=7; j>= 0; j--, line >>= 1) {
                        if(line & 1) {
                            if(size == 1) drawPixel(x+8-j, y+i, color);
                            else drawFillRect(x+8*size-j*size, y+i*size, size, size, color);
                        }
                    }
                }
                break;
            case 2:
                for(int8_t i=startY; i<(characterWidth*endY); i++ ) {
                    uint8_t line = glyph.glyphData[i];
                    for(int8_t j=7; j>= 0; j--, line >>= 1) {
                        if(line & 1) {
                            if(size == 1) drawPixel(x+8-(j+(i%2?8:0)), y+i/2, color);
                            else drawFillRect(x+8*size-(j+(i%2?8:0))*size, y+(i/2)*size, size, size, color);
                        }
                    }
                }
                break;
        }
    } else {
        switch (characterWidth) {
            case 1:
                for(int8_t i=startY; i<(characterWidth*endY); i++ ) {
                    uint8_t line = glyph.glyphData[i];
                    for(int8_t j=7; j>= 0; j--, line >>= 1) {
                        if(line & 1) {
                            if(size == 1) drawPixel(x+j, y+i, color);
                            else drawFillRect(x+j*size, y+i*size, size, size, color);
                        }
                    }
                }
                break;
            case 2:
                for(int8_t i=startY; i<(characterWidth*endY); i++ ) {
                    uint8_t line = glyph.glyphData[i];
                    for(int8_t j=7; j>= 0; j--, line >>= 1) {
                        if(line & 1) {
                            if(size == 1) drawPixel(x+j+(i%2?8:0), y+i/2, color);
                            else drawFillRect(x+(j+(i%2?8:0))*size, y+(i/2)*size, size, size, color);
                        }
                    }
                }
                break;
        }
    }

    return width * this->textSize;
}

size_t BabelTypesetter::writeCodepoint(BABEL_CODEPOINT codepoint) {
    // before we start, we don't need to fetch anything for control characters.
    switch (codepoint) {
        case '\n':
            this->cursor.y += this->paragraphSpacing;
            // fall through
        case '\r':
            this->cursor.y += 16 * this->textSize + this->lineSpacing;
            if (this->direction == 1) {
                this->cursor.x = this->minX;
            } else {
                this->cursor.x = this->maxX;
            }
            this->hasLastGlyph = false;
            return 1;
            this->hasLastGlyph = false;
            return 1;
        case 0x0f: // shift in
            if (this->bold) {
                this->italic = true;
            } else if (this->italic) {
                this->bold = true;
                this->italic = false;
            } else {
                this->italic = true;
            }
            return 1;
        case 0x0e: // shift out
            if (this->bold && this->italic) {
                this->italic = false;
            } else if (this->bold) {
                this->bold = false;
            } else {
                this->italic = false;
            }
            return 1;
        case 0x1e: // record separator
        case 0xfeff: // record separator
            return 1; // ignore it
    }

    BabelGlyph glyph;
    this->babelDevice->fetch_glyph_data(codepoint, &glyph);

    if (this->direction == 1 && BABEL_INFO_GET_STRONG_RTL(glyph.info)) {
        direction = -1;
        uint8_t width = BABEL_INFO_GET_GLYPH_WIDTH(glyph.info);
        this->hasLastGlyph = false;
        this->cursor.x = this->maxX - width;
    }
    else if (this->direction == -1 && BABEL_INFO_GET_STRONG_LTR(glyph.info)) {
        direction = 1;
        this->hasLastGlyph = false;
        this->cursor.x = this->minX;
    }

    // word wrap should go here
    if (BABEL_INFO_GET_MARK_IS_NON_SPACING(glyph.info) && this->hasLastGlyph) {
        // Draw over the last glyph, and do not add to advance
        drawGlyph(this->lastGlyphPosition.x, this->lastGlyphPosition.y, glyph, this->textColor, this->textSize);
    } else {
        // stash current cursor position
        this->hasLastGlyph = true;
        this->lastGlyphPosition = this->cursor;
        // draw glyph
        int advance = drawGlyph(this->cursor.x, this->cursor.y, glyph, this->textColor, this->textSize);
        // advance cursor
        this->cursor.x += advance * this->direction;
    }

    return 1;
}

size_t BabelTypesetter::writeCodepoints(BABEL_CODEPOINT codepoints[], size_t len) {
    size_t retVal = 0;
    
    if (this->lineWidth == 0) {
        // no need to worry about word wrap
        this->hasLastGlyph = false;
        for(size_t i = 0; i < len; i++) {
            retVal += this->writeCodepoint(codepoints[i]);
        }
    } else {
        // TODO: make word wrapping a boolean and just use page margins.
        size_t pos = 0;
        while (pos < len) {
            bool write_newline = false;
            int32_t num_glyphs_to_draw = this->babelDevice->word_wrap_position(codepoints + pos, len - pos, this->lineWidth);
            if (num_glyphs_to_draw < 0){
                num_glyphs_to_draw = (int32_t)(len - pos);
            }
            else {
                write_newline = true;
            }
            for(size_t i = pos; i < pos + num_glyphs_to_draw; i++) {
                retVal += this->writeCodepoint(codepoints[i]);
            }
            pos += num_glyphs_to_draw;
            if (write_newline) {
                retVal += this->writeCodepoint(0x000d); // carriage return (soft wrap)
            }

            if (this->cursor.y >= this->maxY) break;
        }
    }

    return retVal;
}

size_t BabelTypesetter::print(const char * utf8String) {
    size_t len = this->babelDevice->utf8_codepoint_length((char *)utf8String);
    BABEL_CODEPOINT *codepoints = (BABEL_CODEPOINT *)malloc(len * sizeof(BABEL_CODEPOINT));
    this->babelDevice->utf8_parse((char *)utf8String, codepoints);
    size_t retVal = this->writeCodepoints(codepoints, len);
    free(codepoints);

    return retVal;
}

BabelDevice * BabelTypesetter::getBabel() {
    return this->babelDevice;
}

void BabelTypesetter::setTextColor(uint16_t textColor) {
    this->textColor = textColor;
}

void BabelTypesetter::setTextSize(uint16_t textSize) {
    this->textSize = textSize;
}

void BabelTypesetter::setItalic(bool italic) {
    this->italic = italic;
}

void BabelTypesetter::setBold(bool bold) {
    this->bold = bold;
}

void BabelTypesetter::setWordWrap(bool wordWrap) {
    this->lineWidth = wordWrap ? this->maxX - this->minX : 0;
}

void BabelTypesetter::setLineSpacing(int8_t spacing) {
    this->lineSpacing = spacing;
}

void BabelTypesetter::setParagraphSpacing(int8_t spacing) {
    this->paragraphSpacing = spacing;
}
