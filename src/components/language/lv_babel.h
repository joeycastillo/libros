/**
 * @file lv_babel.h
 *
 */
#if 0 // skip this file for now

#ifndef _LV_FONT_BABEL_H
#define _LV_FONT_BABEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "BabelDevice.h"

int lv_babel_init(BabelDevice *babel);
int lv_babel_font_init(lv_font_t * font);

/**********************
 *      MACROS
 **********************/
 
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

#endif
