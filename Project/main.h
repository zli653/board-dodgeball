// Student 1: Yan Xiao
// Student 2: Zeming Li
// Team number: 32

#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <Math.h>

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
#include "ft6x06.h"
#include "screen.h"
#include "ep.h"
#include "wireless.h"

#define BUTTON_UP		0x1
#define BUTTON_DOWN 0x2
#define BUTTON_LEFT 0x4
#define BUTTON_RIGHT 0x8
#define BUTTON_SW2	0x10

// Game data global
#define PLAYER_Y_BASE 242 
#define BAR_TOP 11
#define	BAR_BUTTON 260
#define POINTS_BUTTON 203




typedef enum 
{
  NOT_TOUCHED,
	NOT_TOUCHED_FIRST,
	IN_TOUCH,
	TOUCHED_FIRST,
} TOUCH_STATES;
#endif
