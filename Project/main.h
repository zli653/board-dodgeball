// Copyright (c) 2015-16, Joe Krachey
// All rights reserved.
//
// Redistribution and use in source or binary form, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in source form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in 
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


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

// #include "team_info.h"


#define ADDR_START 	 256
// 80 characters for each field
#define NUM_BYTES	 80
// unsed: info for further usage
#define NUM_FIELD    3
#define BUTTON_UP		0x1
#define BUTTON_DOWN 0x2
#define BUTTON_LEFT 0x4
#define BUTTON_RIGHT 0x8
#define BUTTON_SW2	0x10

// Game data global
#define SELECT_Peashooter 1
#define SELECT_Tall_nut 2
#define SELECT_Cherry_Bomb 3

#endif
