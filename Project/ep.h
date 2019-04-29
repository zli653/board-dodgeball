#ifndef __EP_H__
#define __EP_H__


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


#define ADDR_START 	 256
// 80 characters for each field
#define NUM_BYTES	 80


void eeprom_write_info(void);
void eeprom_print_info(void);
#endif
