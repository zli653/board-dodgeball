#include "screen.h"

void LCD_map_init(void){
	prepare_array();
	lcd_draw_image(
									120,                 // X Pos
                  bitmapDownWidthPixels,   // Image Horizontal Width
                  210,                 // Y Pos
                  bitmapDownHeightPixels,  // Image Vertical Height
                  bitmapDownBitmaps,       // Image
                  LCD_COLOR_RED,      // Foreground Color
                  LCD_COLOR_YELLOW     // Background Color
                );
	lcd_draw_image( 
									120,                 // X Pos
                  bitmapUpWidthPixels,   // Image Horizontal Width
                  70,                 // Y Pos
                  bitmapUpHeightPixels,  // Image Vertical Height
                  bitmapUpBitmaps,       // Image
                  LCD_COLOR_RED,      // Foreground Color
                  LCD_COLOR_YELLOW     // Background Color
                );
	lcd_draw_image( 
									120,                 // X Pos
                  start_gameWidthPixels,   // Image Horizontal Width
                  300,                 // Y Pos
                  start_gameHeightPixels,  // Image Vertical Height
                  start_gameBitmaps,       // Image
                  LCD_COLOR_WHITE,      // Foreground Color
                  LCD_COLOR_BLACK     // Background Color
                );
	LCD_draw_player(120,242);
}

void LCD_draw_player(uint16_t x_pos, uint16_t y_pos){
		lcd_draw_image( 
									x_pos,                 // X Pos
                  playerWidthPixels,   // Image Horizontal Width
                  y_pos,                 // Y Pos
                  playerHeightPixels,  // Image Vertical Height
                  playerBitmaps,       // Image
                  LCD_COLOR_BLACK,      // Foreground Color
                  LCD_COLOR_YELLOW     // Background Color
                );
}

void LCD_delete_player(uint8_t lane){
	if (lane == 0){
		lcd_draw_block(37, 75,232,78,LCD_COLOR_YELLOW);
	} else if (lane == 1){
		lcd_draw_block(118, 74,232,78,LCD_COLOR_YELLOW);
	} else if (lane == 2){
		lcd_draw_block(200, 79,232,78,LCD_COLOR_YELLOW);
	}
}

void LCD_update_score(uint16_t score)
{
	uint8_t first = score % 10;
	uint8_t second = (score / 10) % 10;
	uint8_t last = (score / 100) % 10;

		// update current score 
		lcd_draw_image( 
									212,                 // X Pos
									numbers_WidthPixels,   // Image Horizontal Width
									300,                 // Y Pos
									numbers_HeightPixels,  // Image Vertical Height
									numbers[last],       // Image
									LCD_COLOR_WHITE,      // Foreground Color
									LCD_COLOR_BLACK     // Background Color
								);
		lcd_draw_image( 
									220,                 // X Pos
									numbers_WidthPixels,   // Image Horizontal Width
									300,                 // Y Pos
									numbers_HeightPixels,  // Image Vertical Height
									numbers[second],       // Image
									LCD_COLOR_WHITE,      // Foreground Color
									LCD_COLOR_BLACK     // Background Color
								);
		lcd_draw_image( 
									228,                 // X Pos
									numbers_WidthPixels,   // Image Horizontal Width
									300,                 // Y Pos
									numbers_HeightPixels,  // Image Vertical Height
									numbers[first],       // Image
									LCD_COLOR_WHITE,      // Foreground Color
									LCD_COLOR_BLACK     // Background Color
								);
}

void LCD_score_init(uint16_t highest){
		uint8_t first = highest % 10;
		uint8_t second = (highest / 10) % 10;
		uint8_t last = (highest / 100) % 10;
		lcd_draw_image( 
									105,                 // X Pos
                  scoreWidthPixels,   // Image Horizontal Width
                  300,                 // Y Pos
                  scoreHeightPixels,  // Image Vertical Height
                  scoreBitmaps,       // Image
                  LCD_COLOR_WHITE,      // Foreground Color
                  LCD_COLOR_BLACK     // Background Color
                );
		lcd_draw_image( 
									103,                 // X Pos
									numbers_WidthPixels,   // Image Horizontal Width
									300,                 // Y Pos
									numbers_HeightPixels,  // Image Vertical Height
									numbers[second],       // Image
									LCD_COLOR_WHITE,      // Foreground Color
									LCD_COLOR_BLACK     // Background Color
								);
		lcd_draw_image( 
									95,                 // X Pos
									numbers_WidthPixels,   // Image Horizontal Width
									300,                 // Y Pos
									numbers_HeightPixels,  // Image Vertical Height
									numbers[last],       // Image
									LCD_COLOR_WHITE,      // Foreground Color
									LCD_COLOR_BLACK     // Background Color
								);
		lcd_draw_image( 
									111,                 // X Pos
									numbers_WidthPixels,   // Image Horizontal Width
									300,                 // Y Pos
									numbers_HeightPixels,  // Image Vertical Height
									numbers[first],       // Image
									LCD_COLOR_WHITE,      // Foreground Color
									LCD_COLOR_BLACK     // Background Color
								);
}

void LCD_draw_bar(bar_type type, uint8_t lane, uint16_t y_pos){
		uint8_t width, height;
		uint16_t x_pos, forColor;
		if (type == LONG_BAR || type == POINTS_BAR){
			x_pos = 38 + lane*80;
			if(lane == 2){
				x_pos += 2;
			}
			if (type == LONG_BAR){
				forColor = LCD_COLOR_BLACK;
			} else{
				forColor = LCD_COLOR_BLUE;
			}
			lcd_draw_image( 
									x_pos,                 // X Pos
                  long_BarWidthPixels,   // Image Horizontal Width
                  y_pos,                 // Y Pos
                  long_BarHeightPixels,  // Image Vertical Height
                  long_BarBitmaps,       // Image
                  forColor,      // Foreground Color
                  LCD_COLOR_YELLOW     // Background Color
                );
		} else {
			if (type == LEFT_BAR){
				x_pos = 18 + lane * 80;
			} else {
				x_pos = 58 + lane * 80;
			}
			if(lane == 2 && type == RIGHT_BAR){
				x_pos += 5;
			}
			lcd_draw_image( 
									x_pos,                 // X Pos
                  short_BarWidthPixels,   // Image Horizontal Width
                  y_pos,                 // Y Pos
                  short_BarHeightPixels,  // Image Vertical Height
                  short_BarBitmaps,       // Image
                  LCD_COLOR_BLACK,      // Foreground Color
                  LCD_COLOR_YELLOW     // Background Color
                );
		
		}
	
		

}
