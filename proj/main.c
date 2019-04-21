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

#include "main.h"
#include "stdlib.h"


//*****************************************************************************
//*****************************************************************************
void init_hardware(void)
{
	// TODO: initial all hardware here
  init_serial_debug(true, true);
  lcd_config_gpio();
  lcd_config_screen();
  lcd_clear_screen(LCD_COLOR_BLACK);   
}
	
//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
	// reset
	// eeprom print name
	// SW2 write name
	
	// wireless connect
		// set up master
		// draw map
		// print string on top edge
	  // wait for user press up bottom
	
		// slave
	  // draw different map
		// print string on top edge
	
	// while start
	// running game
	// master
	  // update sunlight sum
	  // get bottom press to change item selection
	  // get touch screen to update item location
	  // transmit sunlight negative change to master
		// update zoombie info (hp, pos, isalive)
		// draw bullet
		// update plant (ho, pos) (and then draw)
		// transmit zoombie dead pos
		
	// slave
	  // random generate sunlight pos
		// get zoombie dead pos
		// update sunlight pos based on accelerometer (reach leftmost edge disapper)
		// collect sunlight at bottom edge
		// tranmit sunlight positive change to master
}
