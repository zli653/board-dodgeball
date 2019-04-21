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

// initial x, y position to the center of the screen
static int CURRENT_X_POS = COLS/2;  // 120
static int CURRENT_Y_POS = ROWS/2;  // 160
// TODO: default stop period?
static int STOP_PERIOD = 0;

//*****************************************************************************
// This is an ARRAY of strings.  If you wanted to access the 3rd string
// ("P10000"), you could do so using COMMANDS[2].
//*****************************************************************************
char * COMMANDS[] = {
    "P200000",
    "R500",
    "P10000",
    "U1000",
    "P5000",
    "L2000",
    "P2500",
    "D2000",
    "P1250",
    "R94",
    "P1",
    "U142"
};

//*****************************************************************************
//*****************************************************************************
void init_hardware(void)
{
  initialize_serial_debug();
  lcd_config_gpio();
  lcd_config_screen();
  lcd_clear_screen(LCD_COLOR_BLACK);   
}

bool rerender_space_ship(int x_pos, int y_pos) {
	bool reach_bound = false;
	// updated x position
	int temp_x_pos = CURRENT_X_POS + x_pos;
	// updated y position
	int temp_y_pos = CURRENT_Y_POS + y_pos;
	int pause_count;
	
	// clearing previous image
	lcd_draw_image(
                  CURRENT_X_POS,                 // X Pos
                  space_shipWidthPixels,   // Image Horizontal Width
                  CURRENT_Y_POS,                 // Y Pos
                  space_shipHeightPixels,  // Image Vertical Height
                  space_shipBitmaps,       // Image
                  LCD_COLOR_BLACK,      // Foreground Color
                  LCD_COLOR_BLACK     // Background Color
                );
	
	// reach the left edge
	if (temp_x_pos - space_shipWidthPixels/2 < 0) {
		// since space_shipWidthPixels is even, does not need to -1 one more time
		// update the x position to let the image at the left most edge
		reach_bound = true;
	}
	// reach the right edge
	else if (temp_x_pos + space_shipWidthPixels/2 >= COLS) {
		// since space_shipWidthPixels is even, does not need to -1 one more time
		// update the x position to let the image at the right most edge
		reach_bound = true;
	}
	// if not reach the left or right edge
	else {
		// update the x position
		CURRENT_X_POS = temp_x_pos;
	}	
	
	// reach the top edge
  if (temp_y_pos - space_shipHeightPixels/2 < 0) {
		// update the y position to let the image at the left most edge
		reach_bound = true;
	}
	// reach the bottom edge
	else if (temp_y_pos + space_shipHeightPixels/2 >= ROWS) {
		// update the y position to let the image at the right most edge
		// since space_shipHeightPixels is even, does not need to -1 one more time
		reach_bound = true;
	}
	// if not reach the top or bottom edge
	else {
		// update the y position		
		CURRENT_Y_POS = temp_y_pos;
	}	
	
	// draw the updated image
	lcd_draw_image(
                  CURRENT_X_POS,                 // X Pos
                  space_shipWidthPixels,   // Image Horizontal Width
                  CURRENT_Y_POS,                 // Y Pos
                  space_shipHeightPixels,  // Image Vertical Height
                  space_shipBitmaps,       // Image
                  LCD_COLOR_RED,      // Foreground Color
                  LCD_COLOR_BLACK     // Background Color
                );
	
	// pause
	for (pause_count = STOP_PERIOD; pause_count > 0; pause_count--) {}
		
	return !reach_bound;
}

void command_parser(char* command) 
{
	char first_letter = command[0];
	
	char* num_str = command + 1;
	// number after the first letter
	uint32_t num = atoi(num_str);
	int count;
	
	switch(first_letter) {
		case 'U':
			// move up
			for (count = 0; count < num; count++) {
				// move the image to the top by 1 pixel each time
				// if reach the boundary, this command is done
				if (!rerender_space_ship(0, -1)) break;
			}	
			break;
		
		case 'D':
			// move down
		  for (count = 0; count < num; count++) {
				// move the image to the bottom by 1 pixel each time
				// if reach the boundary, this command is done
				if (!rerender_space_ship(0, 1)) break;
			}
			break;
		
		case 'L':
			// move left
			for (count = 0; count < num; count++) {
				// move the image to the left by 1 pixel each time
				// if reach the boundary, this command is done
				if (!rerender_space_ship(-1, 0)) break;
			}
			break;
		
		case 'R':
			// move right
			for (count = 0; count < num; count++) {
				// move the image to the right by 1 pixel each time
				// if reach the boundary, this command is done
				if (!rerender_space_ship(1, 0)) break;
			}
			break;
		
		case 'P':
			// set the pause duration
			STOP_PERIOD = num;
			break;
		
		default:
			// ignore unsupported command
			break;
	}	
}	
	
//*****************************************************************************
//*****************************************************************************
int 
main(void)
{
	int i;

  init_hardware();
    
  put_string("\n\r");
  put_string("******************************\n\r");
  put_string("ECE353 HW2 Fall 2018\n\r");
  put_string("Zeming Li\n\r");
  put_string("******************************\n\r");    
  
	// space ship was first located at the center
	rerender_space_ship(0, 0);
	
  // Reach infinite loop
  while(1){
		for (i = 0; i < 12; i++) {
//		put_string("command: ");
//		put_string(COMMANDS[i]);
//		put_string(" \n\r");	
			command_parser(COMMANDS[i]);
		}
		
	};
}
