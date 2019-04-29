#ifndef __WORDS_H__
#define __WORDS_H__

#include <stdint.h>

/* Bitmap info for space_ship */
extern const uint8_t scoreBitmaps[];
extern const uint8_t start_gameBitmaps[];

extern const uint8_t scoreWidthPixels;
extern const uint8_t scoreHeightPixels;

extern const uint8_t start_gameWidthPixels;
extern const uint8_t start_gameHeightPixels;


/* Bitmap info for _0 */
extern uint8_t *numbers[];
extern const uint8_t numbers_WidthPixels;
extern const uint8_t numbers_HeightPixels;

void prepare_array(void);

#endif
