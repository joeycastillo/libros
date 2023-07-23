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

#include <string.h>
#include "BabelDevice.h"
#include "utf8_decode.h"

#define BABEL_HEADER_LOC_RESERVED 0
#define BABEL_HEADER_LOC_VERSION 2
#define BABEL_HEADER_LOC_WIDTH 4
#define BABEL_HEADER_LOC_HEIGHT 5
#define BABEL_HEADER_LOC_FLAGS 6
#define BABEL_HEADER_LOC_START_OF_GLYPHS 8
#define BABEL_HEADER_LOC_MINGLYPH 12
#define BABEL_HEADER_LOC_MAXGLYPH 14
#define BABEL_HEADER_LOC_START_OF_LUT 16
#define BABEL_HEADER_LOC_START_OF_EXTRAS 148

#define BABEL_HEADER_EXTRA_TYPE_UPPERCASE_MAPPINGS 1
#define BABEL_HEADER_EXTRA_TYPE_LOWERCASE_MAPPINGS 2
#define BABEL_HEADER_EXTRA_TYPE_TITLECASE_MAPPINGS 3
#define BABEL_HEADER_EXTRA_TYPE_MIRRORING_MAPPINGS 4


bool BabelDevice::begin() {
    uint16_t val;
    this->read(BABEL_HEADER_LOC_RESERVED, &val, sizeof(val));
    if (val != 0) return false;
    this->read(BABEL_HEADER_LOC_VERSION, &val, sizeof(val));
    if (val != 1) return false;
    this->read(BABEL_HEADER_LOC_WIDTH, &this->width, sizeof(this->width));
    this->read(BABEL_HEADER_LOC_HEIGHT, &this->height, sizeof(this->height));
    this->read(BABEL_HEADER_LOC_MAXGLYPH, &this->last_codepoint, sizeof(this->last_codepoint));
    this->read(BABEL_HEADER_LOC_START_OF_LUT, &this->location_of_lut, sizeof(this->location_of_lut));
    this->read(BABEL_HEADER_LOC_START_OF_GLYPHS, &this->location_of_glyphs, sizeof(this->location_of_glyphs));
    this->read(BABEL_HEADER_LOC_START_OF_EXTRAS, &this->location_of_extras, sizeof(this->location_of_extras));
    uint32_t extra_loc = 0;
    uint32_t extra_len = 0;
    uint32_t currentPos = this->location_of_extras;
    do {
        this->read(currentPos, &extra_loc, sizeof(extra_loc));
        currentPos += sizeof(extra_loc);
        this->read(currentPos, &extra_len, sizeof(extra_loc));
        currentPos += sizeof(extra_len);
        switch (extra_loc & 0xFF) {
        case BABEL_HEADER_EXTRA_TYPE_UPPERCASE_MAPPINGS:
            this->start_of_uppercase_mapping = extra_loc >> 8;
            this->end_of_uppercase_mapping = (extra_loc >> 8) + (extra_len >> 8);
            break;
        case BABEL_HEADER_EXTRA_TYPE_LOWERCASE_MAPPINGS:
            this->start_of_lowercase_mapping = extra_loc >> 8;
            this->end_of_lowercase_mapping = (extra_loc >> 8) + (extra_len >> 8);
            break;
        case BABEL_HEADER_EXTRA_TYPE_TITLECASE_MAPPINGS:
            this->start_of_titlecase_mapping = extra_loc >> 8;
            this->end_of_titlecase_mapping = (extra_loc >> 8) + (extra_len >> 8);
            break;
        case BABEL_HEADER_EXTRA_TYPE_MIRRORING_MAPPINGS:
            this->start_of_mirrored_mapping = extra_loc >> 8;
            this->end_of_mirrored_mapping = (extra_loc >> 8) + (extra_len >> 8);
            break;
        }
    } while(extra_loc && (currentPos < 256));
    
    this->info_for_replacement_character = this->fetch_glyph_basic_info(0xFFFD);
    this->extended_info_for_replacement_character = this->fetch_glyph_extended_info(0xFFFD);

    return true;
}

BABEL_CODEPOINT BabelDevice::get_last_available_codepoint() {
    return this->last_codepoint;
}

uint8_t BabelDevice::getHeight() {
    return this->height;
}

uint32_t BabelDevice::fetch_glyph_basic_info(BABEL_CODEPOINT codepoint) {
    uint32_t retVal;
    uint32_t loc = this->location_of_lut + codepoint * 6;

    this->read(loc, &retVal, 4);

    return retVal;
}

uint16_t BabelDevice::fetch_glyph_extended_info(BABEL_CODEPOINT codepoint) {
    uint16_t retVal;
    uint32_t loc = 4 + this->location_of_lut + codepoint * 6;

    this->read(loc, &retVal, 2);

    return retVal;
}

bool BabelDevice::fetch_glyph_data(BABEL_CODEPOINT codepoint, BabelGlyph *glyph) {
    bool retVal = true;
    uint32_t loc = this->location_of_lut + codepoint * 6;

    // don't bother looking up out-of-range codepoints
    if (codepoint > this->last_codepoint) glyph->info = 0;
    else this->read(loc, glyph, 6);

    if (!glyph->info) {
        glyph->info = this->info_for_replacement_character;
        glyph->extendedInfo = this->extended_info_for_replacement_character;
        retVal = false;
    }
    
    loc = BABEL_INFO_GET_GLYPH_LOCATION(glyph->info);

    if (BABEL_INFO_GET_GLYPH_WIDTH(glyph->info) > 8) {
        this->read(loc, &glyph->glyphData, 32);
        return retVal;
    } else {
        this->read(loc, &glyph->glyphData, 16);
        return retVal;
    }
}

size_t BabelDevice::utf8_codepoint_length(char string[]) {
    return this->utf8_parse(string, NULL);
}

size_t BabelDevice::utf8_parse(char * string, BABEL_CODEPOINT *buf) {
    utf8_decode_init(string, strlen(string));
    size_t len = 0;
    
    do
    {
        int c = utf8_decode_next();
        if (c == UTF8_END) break;
        else if (c == UTF8_ERROR) return -1;
        
        if (buf != NULL) buf[len++] = (uint16_t)c;
        else len++;
    } while (1);
    
    return len;
}

void BabelDevice::to_uppercase(BABEL_CODEPOINT *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        uint16_t extendedInfo = this->fetch_glyph_extended_info(buf[i]);
        if (BABEL_EXTENDED_GET_HAS_UPPERCASE_MAPPING(extendedInfo)) {
            BABEL_CODEPOINT uppercaseCodepoint = this->uppercase_mapping_for_codepoint(buf[i]);
            buf[i] = uppercaseCodepoint;
        }
    }
}

void BabelDevice::to_lowercase(BABEL_CODEPOINT *buf, size_t len) {
    for (size_t i = 0; i < len; i++) {
        uint16_t extendedInfo = this->fetch_glyph_extended_info(buf[i]);
        if (BABEL_EXTENDED_GET_HAS_LOWERCASE_MAPPING(extendedInfo)) {
            BABEL_CODEPOINT lowercaseCodepoint = this->lowercase_mapping_for_codepoint(buf[i]);
            buf[i] = lowercaseCodepoint;
        }
    }
}

int16_t BabelDevice::search_mapping(uint32_t start_of_mapping, uint32_t first, uint32_t last, BABEL_CODEPOINT key) {
    uint32_t retVal;
    if (first > last) {
        retVal = -1;
    } else {
        uint32_t mid = (first + last)/2;
        BABEL_MAPPING mapping;
        this->read(start_of_mapping + mid * sizeof(BABEL_MAPPING), &mapping, sizeof(BABEL_MAPPING));
        BABEL_CODEPOINT current_key = BABEL_MAPPING_GET_KEY(mapping);
        if (current_key == key) {
            retVal = mid;
        } else {
            if (key < current_key) {
                retVal = this->search_mapping(start_of_mapping, first, mid - 1, key);
            } else {
                retVal = this->search_mapping(start_of_mapping, mid + 1, last, key);
            }
        }
    }

    return retVal;
}

BABEL_CODEPOINT BabelDevice::uppercase_mapping_for_codepoint(BABEL_CODEPOINT codepoint) {
    uint32_t lastIndex = (this->end_of_uppercase_mapping - this->start_of_uppercase_mapping) / sizeof(BABEL_MAPPING);
    int16_t index_of_result = search_mapping(this->start_of_uppercase_mapping, 0, lastIndex, codepoint);

    if (index_of_result == -1) return codepoint;
    BABEL_MAPPING mapping;
    this->read(this->start_of_uppercase_mapping + index_of_result * sizeof(BABEL_MAPPING), &mapping, sizeof(BABEL_MAPPING));

    return BABEL_MAPPING_GET_VALUE(mapping);
}

BABEL_CODEPOINT BabelDevice::lowercase_mapping_for_codepoint(BABEL_CODEPOINT codepoint) {
    uint32_t lastIndex = (this->end_of_lowercase_mapping - this->start_of_lowercase_mapping) / sizeof(BABEL_MAPPING);
    int16_t index_of_result = search_mapping(this->start_of_lowercase_mapping, 0, lastIndex, codepoint);

    if (index_of_result == -1) return codepoint;
    BABEL_MAPPING mapping;
    this->read(this->start_of_lowercase_mapping + index_of_result * sizeof(BABEL_MAPPING), &mapping, sizeof(BABEL_MAPPING));

    return BABEL_MAPPING_GET_VALUE(mapping);
}

int16_t BabelDevice::word_wrap_position(BABEL_CODEPOINT *buf, size_t len, bool *wrapped, size_t *wrap_candidate_bytes, int16_t line_width, int16_t text_size) {
    size_t wrap_candidate = 0;
    size_t byte_position = 0;
    size_t position_in_string = 0;
    int16_t cursor_location = 0;
    *wrapped = true; // assume we wrapped unless set otherwise below
    
    while(cursor_location < line_width) {
        if (buf[position_in_string] == '\n') {
            *wrapped = false;
            *wrap_candidate_bytes = byte_position + 1;
            return position_in_string + 1; // "wrap" at the newline
        }
        // skip control characters
        if (buf[position_in_string] < 0x20) {
            byte_position++;
            position_in_string++;
            continue;
        }

        uint32_t glyph_info = this->fetch_glyph_basic_info(buf[position_in_string]);
        if (BABEL_INFO_GET_LINEBREAK_OPPORTUNITY(glyph_info)) {
            wrap_candidate = position_in_string;
            *wrap_candidate_bytes = byte_position;
        }
        if (!(BABEL_INFO_GET_MARK_IS_NON_SPACING(glyph_info) || BABEL_INFO_GET_CONTROL_CHARACTER(glyph_info))) {
            cursor_location += BABEL_INFO_GET_GLYPH_WIDTH(glyph_info) * text_size;
        }
        // this first one is always false for 16-bit codepoints, but could be relevant for 32-bit codepoints
        // if (buf[position_in_string] > 0x00ffff) byte_position++;
        if (buf[position_in_string] > 0x0007ff) byte_position++;
        if (buf[position_in_string] > 0x00007f) byte_position++;
        byte_position++;
        position_in_string++;
        if (position_in_string >= len) {
            *wrapped = false;
            // FIXME: byte position and wrap position?
            return -1; // we didn't have to word wrap
        }
    }
    
    if (wrap_candidate) return wrap_candidate + 1; // if we found a wrap point, return it (plus the space after).
    else return len; // otherwise, they'll just need to break at the end of the line even though it's in the middle of a word.
}
