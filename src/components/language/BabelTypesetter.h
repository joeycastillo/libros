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

#ifndef BabelTypesetter_h
#define BabelTypesetter_h

#include <stdio.h>
#include <stdint.h>
#include "BabelDevice.h"

class BabelTypesetter {
public:
    struct Point {
        int16_t x = 0;
        int16_t y = 0;
    };

    BabelTypesetter();
    virtual void begin();
    /**
     @brief Pure virtual method for drawing a pixel. All glyph drawing methods call this to push pixels.
     @param x pixel's X coordinate
     @param y pixel's Y coordinate
     @param color 16-bit pixel color
    */
    virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;    ///< Virtual drawPixel() function to draw to the screen/framebuffer/etc, must be overridden in subclass. 

    /**
     @brief Virtual method for drawing a rect. You may override this if you have a more efficient implementation.
     @param x origin X coordinate
     @param y origin Y coordinate
     @param w rect width
     @param h rect height
     @param color 16-bit pixel color
    */
    virtual void drawFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

    /*!
     @brief sets cursor position
     @param x x coordinate
     @param y y coordinate
    */
    /**************************************************************************/
    void setCursor(int16_t x, int16_t y);

    /*!
     @brief gets cursor X position
    */
    /**************************************************************************/
    int16_t getCursorX();

    /*!
     @brief gets cursor Y position
    */
    /**************************************************************************/
    int16_t getCursorY();

    /*!
     @brief resets cursor position to origin of layout area
    */
    /**************************************************************************/
    void resetCursor();

    /*!
     @brief sets the area where glyphs can be drawn
     @param x x origin of layout rect
     @param y y origin of layout rect
     @param w width of layout rect
     @param h height of layout rect
     @note OK! SO. The typesetter would LOVE to be completely agnostic about what kind of device it's rendering to. It would, like, love nothing more than to not ever have to query the underlying object that's `drawPixel`'ing. So if you have a 128x128 TFT and you want it to use the whole thing, have at! Pass in `0, 0, 128, 128` and typesetter will call your drawPixel method on all those pixels. For the 400x300 book, I'm probably going to specify some tasteful margins like `16, 16, 268, 368`.
    */
    /**************************************************************************/
    void setLayoutArea(int16_t x, int16_t y, int16_t w, int16_t h);

    /*!
     @brief Draws a glyph at the given coordinates.
     @param x X coordinate of the glyph.
     @param y Y coordinate of the glyph.
     @param glyph The glyph you wish to draw.
     @param color 16-bit pixel color
     @returns the width of the glyph that was drawn. Can be zero.
     @note This method simply draws the glyph; it does not touch layout direction or cursor position.
    */
    int drawGlyph(int16_t x, int16_t y, BabelGlyph glyph, uint16_t color, uint8_t size);

    /*!
     @brief Writes a glyph at the current cursor position
     @param codepoint The codepoint you wish to draw. Not UTF-8. Not UTF-16. The codepoint itself.
     @returns the number 1 if a codepoint was written, 0 if one was not.
     @note This method handles newlines and direction changes, and updates the current cursor position. It might move 8 or 16 pixels to the right, OR it might move to the left side of the next line if the text wrapped. But it could also move to the right side of the next line if the layout direction changed to RTL mode.
    */
    size_t writeCodepoint(BABEL_CODEPOINT codepoint);

    /*!
     @brief Writes a series of glyphs at the current cursor position. It will not currently wrap, but will advance the line for newlines, and automatically change the layout mode to RTL or LTR as appropriate.
     @param codepoints An array of codepoints that you wish to draw
     @param len The number of codepoints in the array
     @returns the number of codepoints written
     @note This method handles newlines and direction changes, and updates the current cursor position. It might move 8 or 16 pixels to the right, OR it might move to the left side of the next line if the text wrapped. But it could also move to the right side of the next line if the layout direction changed to RTL mode.
    */
    size_t writeCodepoints(BABEL_CODEPOINT codepoints[], size_t len);

    /*!
     @brief Prints a UTF-8 string at the current cursor position. It will not currently wrap, but will advance the line for newlines, and automatically change the layout mode to RTL or LTR as appropriate.
     @param utf8String a NULL-terminated UTF-8 string
     @returns the number of codepoints printed
     @note This method handles newlines and direction changes, and updates the current cursor position. It might move 8 or 16 pixels to the right, OR it might move to the left side of the next line if the text wrapped. But it could also move to the right side of the next line if the layout direction changed to RTL mode.
    */
    size_t print(char * utf8String);

    /*!
     @brief access to the Babel abstraction, for things like getting glyphs, case mapping, word wrapping, etc.
    */
    /**************************************************************************/
    BabelDevice *getBabel();

    /*!
     @brief sets text color
     @param textColor the color you want to set
    */
    /**************************************************************************/
    void setTextColor(uint16_t textColor);

    /*!
     @brief sets text size
     @param textSize a size multiplier (2x, 3x, etc)
    */
    /**************************************************************************/
    void setTextSize(uint16_t textSize);

    /*!
     @brief sets or unsets italic mode
     @param italic true if the text should be drawn in italic, false for non-italic
     @note We don't actually have italic glyphs; instead we offset the lines of the glyph to the right by up to three pixels. This means that the text may extend outside the normal bounding box. This may also impact readability for some languages and symbols; make sure to test your intended use.
    */
    /**************************************************************************/
    void setItalic(bool italic);

    /*!
     @brief sets or unsets bold mode
     @param bold true if the text should be drawn in bold, false for non-bold
     @note We don't actually have boldface glyphs; instead, we double-stroke the glyph, one pixel to the right. This means that the text may extend outside the normal bounding box. This may also impact readability for some languages and symbols; make sure to test your intended use.
    */
    /**************************************************************************/
    void setBold(bool bold);

    /*!
     @brief sets the line width for word wrapping
     @param lineWidth 0 to disable word wrapping, or the max width in pixels of a line
    */
    /**************************************************************************/
    void setWordWrap(bool wordWrap);
protected:
    BabelDevice *babelDevice = NULL;
    uint16_t textColor = 0;
    uint16_t textSize = 1;
    bool italic = false;
    bool bold = false;
    uint16_t lineWidth = 0;
    Point cursor;
    int16_t minX = 0;
    int16_t minY = 0;
    int16_t maxX = 0;
    int16_t maxY = 0;
private:
    int drawGlyph(int16_t x, int16_t y, BabelGlyph glyph, uint16_t color, uint8_t size, uint8_t startY, uint8_t endY);
    // layout direction. 1 for LTR, -1 for RTL.
    int8_t  direction = 1;
    // these next two are for combining and enclosing marks, which should be drawn atop the previously drawn glyph.
    bool hasLastGlyph = false;
    Point lastGlyphPosition;
};

#endif /* BabelTypesetter_h */
