/**
 * @file lv_babel.c
 *
 */
#if 0 // skip this file for now

#include "lv_babel.h"

static BabelDevice *lv_babel = NULL;
static BabelGlyph lv_babel_glyph;

static bool get_glyph_dsc_cb(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next)
{
    if (unicode_letter > lv_babel->get_last_available_codepoint())
    {
        unicode_letter = 0xFFFD; // return the Unicode replacement character
    }

    uint32_t info = lv_babel->fetch_glyph_basic_info(unicode_letter);
    
    dsc_out->adv_w = BABEL_INFO_GET_MARK_IS_NON_SPACING(info) ? 0 : BABEL_INFO_GET_GLYPH_WIDTH(info);
    dsc_out->box_h = lv_babel->getHeight();
    dsc_out->box_w = BABEL_INFO_GET_GLYPH_WIDTH(info);
    dsc_out->ofs_x = 0;
    dsc_out->ofs_y = 0;
    dsc_out->bpp = 1;

    return true;
}

/* Get the bitmap of `unicode_letter` from `font`. */
static const uint8_t * get_glyph_bitmap_cb(const lv_font_t * font, uint32_t unicode_letter)
{
    if (unicode_letter > lv_babel->get_last_available_codepoint())
    {
        unicode_letter = 0xFFFD; // return the Unicode replacement character
    }
    lv_babel->fetch_glyph_data(unicode_letter, &lv_babel_glyph);

    return (const uint8_t *)&(lv_babel_glyph.glyphData);
}

/**
* init babel device
* @param babel instance of a concrete BabelDevice subclass such as BabelFile or BabelSPIFlash
* @return 0 for success, 1 for failure
*/
int lv_babel_init(BabelDevice *babel)
{
    lv_babel = babel;
    lv_babel->begin();

    return lv_babel->get_last_available_codepoint() == 0;
}

/**
* init lv_font_t struct
* @param font pointer to a font
* @return 0 for success, 1 for failure
*/
int lv_babel_font_init(lv_font_t * font)
{
    font->get_glyph_dsc = get_glyph_dsc_cb;        /* Set a callback to get info about gylphs */
    font->get_glyph_bitmap = get_glyph_bitmap_cb;  /* Set a callback to get bitmap of a glyph */

    font->line_height = lv_babel->getHeight();
    font->base_line = 0;
    font->subpx = LV_FONT_SUBPX_NONE;
    
    return font->line_height == 0;
}

#endif