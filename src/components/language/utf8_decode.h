/* utf8_decode.h */

#ifndef utf8_decode_h
#define utf8_decode_h

#include <stdio.h>
#include "BabelMacros.h"

#define UTF8_END 0xFFFF
#define UTF8_ERROR 0xFFFE
#define UTF8_REPLACEMENT_CHARACTER 0xFFFD


int utf8_decode_at_byte(void);
int utf8_decode_at_character(void);
void utf8_decode_init(char p[], size_t length);
BABEL_CODEPOINT utf8_decode_next(void);

#endif // utf8_decode_h
