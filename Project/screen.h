#ifndef __SCREEN_H__
#define __SCREEN_H__


#include "TM4C123.h"
#include "gpio_port.h"
#include "lcd.h"
#include "lcd_images.h"
#include "eeprom.h"
#include "serial_debug.h"
#include "io_expander.h"
#include "timers.h"
#include "the_map.h"
#include "words.h"
#include "driver_defines.h"
#include "gpio_port.h"
#include "uart.h"
#include "accel.h"
#include "screen.h"

typedef enum {
  LONG_BAR, 
  RIGHT_BAR, 
  LEFT_BAR,
	POINTS_BAR
} bar_type;

void LCD_map_init(void);
void LCD_draw_player(uint16_t x_pos, uint16_t y_pos);
void LCD_update_score(uint16_t score);
void LCD_score_init(uint16_t highest);
void LCD_draw_bar(bar_type type, uint8_t lane, uint16_t y_pos);
#endif
