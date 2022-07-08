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

#ifndef BabelDevice_h
#define BabelDevice_h

#include <stdio.h>
#include <stdint.h>
#include "BabelMacros.h"

struct BabelGlyph {
    uint32_t info;
    uint16_t extendedInfo;
    uint8_t glyphData[32];
};

class BabelDevice {
public:
    virtual bool begin();
    /**
     @brief Returns the last codepoint that is available on the device. Implementations should not request codepoints higher than this.
    */
    BABEL_CODEPOINT get_last_available_codepoint();
    
    /**
     @brief Just an accessor for the declared native height of the font on the chip.
     @return the height of all glyphs in pixels
     */
    uint8_t getHeight();

    /**
     @brief This method seeks into a lookup table for basic data about the glyph. O(1) access time.
     @return A uint32_t that contains the glyph's location on the device in the lower 22 bits, and some basic metadata in the higher 10 bits.
     @param codepoint The codepoint whose basic data you want to fetch.
     @note The macros in BabelMacros.h help to get the desired data out of this value.
     @see BABEL_INFO_GET_GLYPH_LOCATION
     @see BABEL_INFO_GET_GLYPH_WIDTH
     @see BABEL_INFO_GET_MARK_IS_NON_SPACING
     @see BABEL_INFO_GET_MIRRORED_IN_RTL
     @see BABEL_INFO_GET_STRONG_RTL
     @see BABEL_INFO_GET_STRONG_LTR
     @see BABEL_INFO_GET_CONTROL_CHARACTER
     @see BABEL_INFO_GET_LINEBREAK_OPPORTUNITY
    */
    uint32_t fetch_glyph_basic_info(BABEL_CODEPOINT codepoint);
    
    /**
     @brief This method seeks into a secondary lookup table for more detailed data about the glyph. O(1) access time.
     @returns A uint16_t that contains a 5-bit value for the character's general category, a 4-bit value for the character's bidirectional class, and several flags that indicate the presence of mappings.
     @param codepoint The codepoint whose extended data you want to fetch.
     @note The macros in BabelMacros.h help to get the desired data out of this value.
     @see BABEL_EXTENDED_GET_CHAR_CATEGORY
     @see BABEL_EXTENDED_GET_BIDI_CLASS
     @see BABEL_EXTENDED_GET_HAS_UPPERCASE_MAPPING
     @see BABEL_EXTENDED_GET_HAS_LOWERCASE_MAPPING
     @see BABEL_EXTENDED_GET_HAS_TITLECASE_MAPPING
     @see BABEL_EXTENDED_GET_HAS_BIDI_MIRRORING_MAPPING
     @see BABEL_EXTENDED_GET_IS_WHITESPACE
    */
    uint16_t fetch_glyph_extended_info(BABEL_CODEPOINT codepoint);
    
    /**
     @brief This method first calls fetch_glyph_basic_info to populate the struct's info field, which includes the glyph data's location. It then seeks to that location and populates the struct's glyphData field with the glyph bitmap.
     @return true if the codepoint was valid; false if it was invalid. If false, the glyph struct will contain data and graphics for codepoint U+FFFD "REPLACEMENT CHARACTER"
     @param codepoint The codepoint whose data you want to fetch.
     @param glyph Output parameter, the struct you wish to populate with data.
     @note If the glyph is only 16 bytes, this method will populate the first 16 bytes of glyphData, and leave the rest alone. In this case, you can expect a width value in info that is <= 8.
     */
    bool fetch_glyph_data(BABEL_CODEPOINT codepoint, BabelGlyph *glyph);
    
    /**
     @brief This method takes a NULL-terminated UTF-8 string and returns its length in BABEL_CODEPOINTS. Should be O(n) time.
     @return the number of codepoints required to represent this string, or 0 if the string was invalid.
     @param string a pointer to a a UTF-8 string
     */
    size_t utf8_codepoint_length(char * string);

    /**
     @brief This method takes a NULL-terminated UTF-8 string and parses it into codepoints, which it places in the buffer pointed to by buf. O(N) time.
     @return the number of codepoints required to represent this string, or 0 if the string was invalid.
     @param string a pointer to a a UTF-8 string
     @param buf output parameter, a pointer to a buffer that will receive the parsed codepoints.
     */
    size_t utf8_parse(char * string, BABEL_CODEPOINT *buf);

    /**
     @brief This method takes an array of codepoints, and modifies it so that all the characters are uppercase. O(n log(m)) where n is the length of the string, and m is the number of uppercase mappings on the device.
     @param buf the array of codepoints to modify in-place.
     @param len the length of the buffer
     @note This method only handles one-to-one mappings. Some single lowercase characters should capitalize to multuple codepoints (i.e. the german "ß" U+00DF should become "SS", U+0053 U+0053) but this method does not handle these cases.
     */
    void to_uppercase(BABEL_CODEPOINT *buf, size_t len);

    /**
     @brief This method takes an array of codepoints, and modifies it so that all the characters are lowercase. O(n log(m)) where n is the length of the string, and m is the number of lowercase mappings on the device.
     @param buf the array of codepoints to modify in-place.
     @param len the length of the buffer
     */
    void to_lowercase(BABEL_CODEPOINT *buf, size_t len);

    /**
     @brief This method locates the mapping to uppercase for a given codepoint. O(log(n)) where n is the number of uppercase mappings on the device.
     @param codepoint the codepoint whose uppercase mapping you are seeking
     @return the codepoint that the provided codepoint should map to. If no mapping was found, the same codepoint that was passed in.
     @see BABEL_EXTENDED_GET_HAS_UPPERCASE_MAPPING
     @note This is a relatively expensive method to call, especially if the character in question has no uppercase mapping. You should first check the extended metadata to determine if a mapping is available, and only call this method if you are certain that one exists.
     */
    BABEL_CODEPOINT uppercase_mapping_for_codepoint(BABEL_CODEPOINT codepoint);

    /**
     @brief This method locates the mapping to lowercase for a given codepoint. O(log(n)) where n is the number of lower mappings on the device.
     @param codepoint the codepoint whose lowercase mapping you are seeking
     @return the codepoint that the provided codepoint should map to. If no mapping was found, the same codepoint that was passed in.
     @see BABEL_EXTENDED_GET_HAS_LOWERCASE_MAPPING
     @note This is a relatively expensive method to call, especially if the character in question has no lowercase mapping. You should first check the extended metadata to determine if a mapping is available, and only call this method if you are certain that one exists.
     */
    BABEL_CODEPOINT lowercase_mapping_for_codepoint(BABEL_CODEPOINT codepoint);

    /**
     @brief Method for determining where to word wrap lines
     @param buf A buffer of BABEL_CODEPOINTS that you want to wrap.
     @param len number of codepoints in buf
     @param line_width the width in pixels that you want to wrap to
     @param text_size scaling factor for the text (1 for 1x, 2 for 2x, etc.)
     @return the position where a newline should be added in order to wrap to a given line length, or -1 if no newline is required.
     */
    int16_t word_wrap_position(BABEL_CODEPOINT *buf, size_t len, bool *wrapped, size_t *bytePosition, int16_t line_width, int16_t text_size);

protected:
    virtual void read(uint32_t addr, void *data, uint32_t len) = 0;
    int16_t search_mapping(uint32_t start_of_mapping, uint32_t first, uint32_t last, BABEL_CODEPOINT key);
private:
    uint8_t width = 0;
    uint8_t height = 0;
    uint16_t last_codepoint = 0;
    uint32_t location_of_lut = 0;
    uint32_t location_of_glyphs = 0;
    uint32_t location_of_extras = 0;
    uint32_t start_of_uppercase_mapping = 0;
    uint32_t start_of_lowercase_mapping = 0;
    uint32_t start_of_titlecase_mapping = 0;
    uint32_t start_of_mirrored_mapping = 0;
    uint32_t end_of_uppercase_mapping = 0;
    uint32_t end_of_lowercase_mapping = 0;
    uint32_t end_of_titlecase_mapping = 0;
    uint32_t end_of_mirrored_mapping = 0;
    uint32_t info_for_replacement_character = 0;
    uint16_t extended_info_for_replacement_character = 0;
};

#endif /* BabelDevice_h */
