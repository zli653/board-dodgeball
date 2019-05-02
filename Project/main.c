// Copyright (c) 2015-18, Joe Krachey
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
#define SEC_ONE     50000000
volatile bool ALERT_TIMER1_LED_UPDATE;
#define MSEC_EIGHT     50000
volatile bool ALERT_TIMER4_ACC_UPDATE;

volatile uint8_t Button_interrupted;
volatile int Button_value;
int Button_old;
volatile int Button_flag;
int Button_real_flag;

uint8_t game_started;

uint16_t player_x;
uint16_t player_y;
uint8_t current_lane;


uint16_t touch_x, touch_y;
int count;
//*****************************************************************************
//*****************************************************************************
void DisableInterrupts(void)
{
	__asm {
		CPSID  I
	}
}
//*****************************************************************************
//*****************************************************************************
void EnableInterrupts(void)
{
	__asm {
		CPSIE  I
	}
}
void boundary_check(uint8_t lane) {
		if (lane == 0){
			if (player_x < 18) {
				player_x = 18; 
			}
			else if (player_x > 56) {
				player_x = 56; 
			}	
		}
		if (lane == 1){
			if (player_x < 100) {
				player_x = 100; 
			}
			else if (player_x > 136) {
				player_x = 136; 
			}	
		}
		if (lane == 2){
			if (player_x < 180) {
				player_x = 180; 
			}
			else if (player_x > 222) {
				player_x = 222; 
			}	
		}
		// 81
	
}

void LCD_clear_bar(uint8_t lane, bar_type types){
		uint8_t width, height;
		uint16_t x_pos, y_pos;
			x_pos = 38 + lane*80;
			if(lane == 2){
				x_pos += 2;
			}
			if (types == POINTS_BAR){
				y_pos = POINTS_BUTTON;
			} else {
				y_pos = BAR_BUTTON;
			}
			lcd_draw_block( 
									x_pos,                 // X Pos
                  long_BarWidthPixels+9,   // Image Horizontal Width
                  y_pos,                 // Y Pos
                  long_BarHeightPixels,  // Image Vertical Height
                  LCD_COLOR_YELLOW     // Background Color
                );
}

typedef enum 
{
  NOT_TOUCHED,
	NOT_TOUCHED_FIRST,
	IN_TOUCH,
	TOUCHED_FIRST,
} TOUCH_STATES;

static TOUCH_STATES state = NOT_TOUCHED;
 
bool touch_fsm_sw(uint8_t current_lane)
{
	uint16_t touch_event;
	touch_event = ft6x06_read_td_status();
	switch (state)
	{
		case NOT_TOUCHED:
		{
			if(touch_event > 0) {state = TOUCHED_FIRST; }
			else { state = NOT_TOUCHED; }
		  return false;
		}
		case TOUCHED_FIRST:
		{
			if(touch_event > 0) {state = IN_TOUCH; 
				touch_x = ft6x06_read_x();
				touch_y = ft6x06_read_y();
					// printf("Jump times: %d,%d\n\r", touch_x, touch_y);
				return true;
			}
			else { state = NOT_TOUCHED; }
		  return false;
		}
		case NOT_TOUCHED_FIRST:
		{
			if(touch_event > 0) {state = IN_TOUCH;}
			else { state = NOT_TOUCHED; }
		  return false;
		}
		case IN_TOUCH:
		{
			if(touch_event > 0) {state = IN_TOUCH;}
			else { state = NOT_TOUCHED_FIRST; }
			return false;
		}
		default:
		{
			while(1){};
		}
		return false;
	}
}

void TIMER1A_Handler(void)
{
	TIMER0_Type *gp_timer = (TIMER0_Type *)TIMER1_BASE;
	// acknowledges (clears) a Timer A timeout
	gp_timer->ICR |= TIMER_ICR_TATOCINT;
	
	ALERT_TIMER1_LED_UPDATE = true;
}

void TIMER4A_Handler(void)
{
	  static int count = 0;

	TIMER0_Type *gp_timer = (TIMER0_Type *)TIMER4_BASE;
	// acknowledges (clears) a Timer A timeout
	gp_timer->ICR |= TIMER_ICR_TATOCINT;
	
	//ALERT_TIMER4_ACC_UPDATE = true;
	if (count == 0)
	{
			ALERT_TIMER4_ACC_UPDATE = true;
			count = 0;
	}
	else
	{
		count++;
	}
}	

bool init_gpio(){
	if(gpio_enable_port(GPIOA_BASE) == false)   return false;
	if(gpio_enable_port(GPIOB_BASE) == false)   return false;
	if(gpio_enable_port(GPIOC_BASE) == false)   return false;
	if(gpio_enable_port(GPIOF_BASE) == false)   return false;
	return true;
}

bool init_i2c() {

	// Configure SCL 
  if(gpio_config_digital_enable(GPIOA_BASE, PA6)== false)  return false;
	if(gpio_config_digital_enable(GPIOA_BASE, PA7)== false)  return false;

    
  if(gpio_config_alternate_function(GPIOA_BASE, PA6)== false)  return false;
	if(gpio_config_alternate_function(GPIOA_BASE, PA7)== false)  return false;
  
	
	if(gpio_config_open_drain(GPIOA_BASE, PA7)== false)  return false;
	
  if(gpio_config_port_control(GPIOA_BASE, GPIO_PCTL_PA6_M, GPIO_PCTL_PA6_I2C1SCL)== false)  return false;
	if(gpio_config_port_control(GPIOA_BASE, GPIO_PCTL_PA7_M, GPIO_PCTL_PA7_I2C1SDA)== false)   return false;

	
	if(initializeI2CMaster(I2C1_BASE)!= I2C_OK)  return false;
	return true;
}

//*****************************************************************************
//*****************************************************************************
bool init_hardware(void)
{
	// initial all hardware here
	DisableInterrupts();
	init_gpio();
	
	if(!init_serial_debug(true, true)) return false;
	if(!init_i2c()) return false;

	lcd_config_screen();
	lcd_clear_screen(LCD_COLOR_BLACK);
	if(!io_expander_init()) return false;
	
	accel_initialize();
	
  // inside initalize hardware
  // Initialize the TIMER1 to be a 
  //      32-bit
  //      Periodic
  //      count up
  //      generate interrupts 
  gp_timer_config_32(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, true, true);
	gp_timer_enable(TIMER1_BASE, SEC_ONE);
  // Initialize the TIMER4A to be a 
  //      16-bit
  //      peridoc
  //      count down
  //      with interrupts
  gp_timer_config_16(TIMER4_BASE, TIMER_TAMR_TAMR_PERIOD, false, true, 7); 
  gp_timer_enable(TIMER4_BASE, MSEC_EIGHT);

	// init globals
	
	Button_interrupted = 0;
	Button_value = 0;
	Button_flag = 0x0;
	Button_old = 0x0f;
	Button_real_flag = 0x00;
	game_started = 0;
	current_lane  = 1;
	// at the end of enable timer
	player_x = 120;
	player_y = PLAYER_Y_BASE;
  
	EnableInterrupts();
  return true;
}

void GPIOF_Handler(void){
	Button_flag = read_interrupt();
	Button_value = read_button();
	Button_interrupted =	1;
	GPIOF->ICR |= 0xff;
}

	
//*********************************************************************************************************************************************************************
//*********************************************************************************************************************************************************************
int 
main(void)
{ 
	bars *game_bar;
	bool jump;
	int jump_count = 0;
	int8_t accel_flag;
	uint32_t i,j;
	uint8_t tick_count,tick_count_t, jump_seq,num_bars_exist,gen_bar_flag,gen_bar_count,game_over_flag;
	bool tick;
	int16_t accel_x, highest_score,points;

	
	init_hardware();
	eeprom_print_info();
	LCD_map_init();
	game_over_flag = 0;
	num_bars_exist = 0;
	gen_bar_flag = 0;
	jump = 0;	
	jump_seq = 0;
	tick = false;
	gen_bar_count = 0;
	//LCD_draw_player(222, player_y);
	// LCD_draw_bar(LONG_BAR, 1, 202);
	eeprom_write_score(0);
	game_bar = malloc(sizeof(bars)*15);
	for(i = 0; i < 15; i++){
		game_bar[i].type = UNUSED;
	}
	while(1){
		
		// 1s timer
		if(ALERT_TIMER1_LED_UPDATE) {
			light_blink();
			if (game_started){
				points++;
				gen_bar_flag = 1;;
			}
			ALERT_TIMER1_LED_UPDATE = false;
    }
		
		// 8ms timer
    if(ALERT_TIMER4_ACC_UPDATE) {
			// if game has started
			if (game_started == 1){
				accel_x = accel_read_x();	
				jump = touch_fsm_sw(current_lane);
				// printf("ACCEL X: %d\n",accel_x);
				if (accel_x > 8000) {
					accel_flag = -3;
				}	
				else if (accel_x > 3000) {
					accel_flag = -2;
				}
				else if (accel_x > 1300) {
					accel_flag = -1;
				}
				else if (accel_x < -8000) {
					accel_flag = 3;
				}
				else if (accel_x < -3000) {
					accel_flag = 2;
				}
				else if (accel_x > 1300) {
					accel_flag = 1;
				} else {
					accel_flag = 0;
				}
			}
			tick = true;
			ALERT_TIMER4_ACC_UPDATE = false;
    }

		// Handle button interrupt
		if (Button_interrupted == 1){
			if (Button_flag == 0){
				Button_real_flag |= 0x10;
			} else {
				Button_real_flag = (Button_old) & (~Button_value);
				Button_old = Button_value;
			}
			Button_interrupted = 0;
		}
		
		
		// react to input
		// No matter game started or not
		// if SW2 is pressed, write name
		
		
		
		// If game has not started
		if (game_started == 0){
			if ((Button_real_flag & BUTTON_SW2) != 0){
				eeprom_write_info();
				Button_real_flag &= BUTTON_SW2;
				
			}
			if ((Button_real_flag & BUTTON_DOWN) != 0){
				for(i = 0; i < 15; i++){
					game_bar[i].type = UNUSED;
				}
				game_started = 1;
				points = 0;
				player_x = 120;
				player_y = PLAYER_Y_BASE;
				current_lane = 1;
				LCD_map_init();
				highest_score = eeprom_print_score();
				LCD_score_init(highest_score);
				Button_real_flag &= ~BUTTON_DOWN;
			}
		} 
		
		// If game has started
		if (game_started ==1){
			// Check jump
			if (jump == 1){
				if (touch_y > 300 || touch_y < 220) jump = 0;
				if (current_lane == 0){
					if (touch_x > 100) jump = 0; 
				}else if (current_lane == 1) {
					if (touch_x > 180 || touch_x < 60 ) jump = 0; 
				}else {
					if (touch_x < 160) jump = 0; 
				}
				// printf("after: %d, %d, %d\n\r", jump,touch_x,touch_y);
			}
			
			if (jump == 1) {
				if (jump_seq == 0){
					jump_seq = 1;
				}
				jump = 0;
			}
			
			if (gen_bar_flag == 1){
				gen_bar_flag = 0;
				if (points >= 100){
					j = rand() % 3;
					for (i = 0; i <= 15; i++){
						if (i == 15) break;
						if (game_bar[i].type == UNUSED) break;
					}
					if (i != 15){
						game_bar[i].type = rand() % 4+1;
						game_bar[i].y_pos = BAR_TOP;
						game_bar[i].lanes = (j+1)%3;
					}
					for (i = 0; i <= 15; i++){
						if (i == 15) break;
						if (game_bar[i].type == UNUSED) break;
					}
					if (i != 15){
						game_bar[i].type = rand() % 4+1;
						game_bar[i].y_pos = BAR_TOP;
						game_bar[i].lanes = (j+2)%3;
					}
					
				}else if (points >= 60){
					for (i = 0; i <= 15; i++){
						if (i == 15) break;
						if (game_bar[i].type == UNUSED) break;
					}
					if (i != 15){
						game_bar[i].type = rand() % 4+1;
						game_bar[i].y_pos = BAR_TOP;
						game_bar[i].lanes = rand() % 3;
					}
				} else {
					gen_bar_count = (gen_bar_count +1) %2;
					if (gen_bar_count == 0){
						for (i = 0; i <= 15; i++){
							if (i == 15) break;
							if (game_bar[i].type == UNUSED) break;
						}
						if (i != 15){
							game_bar[i].type = rand() % 4+1;
							game_bar[i].y_pos = BAR_TOP;
							game_bar[i].lanes = rand() % 3;
						}
					}
				
				}
			}
			
			// check change lanes
			if ((Button_real_flag & BUTTON_LEFT) != 0){
				if (current_lane >= 1){
					LCD_delete_player(current_lane);
					current_lane = current_lane - 1;
					player_x -= 80;
				}
				Button_real_flag &= ~BUTTON_LEFT;
			} else if ((Button_real_flag & BUTTON_RIGHT) != 0){
				if (current_lane <= 1){
					LCD_delete_player(current_lane);
					current_lane = current_lane + 1;
					player_x += 80;
				}
				Button_real_flag &= ~BUTTON_RIGHT;
			}
			
			// update score
			LCD_update_score(points);
			
			// undate image based on each tick
			if (tick){
				
				tick_count = (tick_count +1)%2;
				tick_count_t = (tick_count_t +1)%3;
				if (tick_count == 0){
					if (jump_seq == 5){
						jump_seq++;
						player_y = PLAYER_Y_BASE -7;
					}else if (jump_seq == 7){
						jump_seq++;
						player_y = PLAYER_Y_BASE -7;
					}
					
					if (accel_flag == 1 || accel_flag == -1){
						player_x --;
					}
					if (points <= 30){
						for (i = 0; i < 15; i++){
							if (game_bar[i].type != POINTS_BAR && game_bar[i].type!= UNUSED){
							if (game_bar[i].y_pos >= BAR_BUTTON){
								LCD_clear_bar(game_bar[i].lanes, game_bar[i].type);
								game_bar[i].type = UNUSED;
								
							}
							game_bar[i].y_pos += 1;
							}else	if (game_bar[i].type == POINTS_BAR){
								if (game_bar[i].y_pos >= POINTS_BUTTON){
									LCD_clear_bar(game_bar[i].lanes, game_bar[i].type);
									game_bar[i].type = UNUSED;
								}
								game_bar[i].y_pos += 2;
							}	
							if (game_bar[i].type != UNUSED){
								LCD_draw_bar(game_bar[i].type,game_bar[i].lanes,game_bar[i].y_pos); 
							}
						}
					}					
					// if update each 2 tick
				}
				
				
				
				if (tick_count_t == 0){
					if (jump_seq == 6){
						jump_seq++;
						player_y = PLAYER_Y_BASE - 8;
					}
				}
				
				if (accel_flag == 2 ||accel_flag == 3 ){
						player_x = player_x +accel_flag -1;
				}
				if (accel_flag == -2|| accel_flag == -3){
						player_x = player_x +accel_flag +1;
				}
				if (jump_seq == 1){
						jump_seq++;
						player_y = PLAYER_Y_BASE -2;
				}else if (jump_seq == 2){
					jump_seq++;
					player_y = PLAYER_Y_BASE -4;
				}else if (jump_seq == 3){
					jump_seq++;
					player_y = PLAYER_Y_BASE -5;
				}else if (jump_seq == 4){
					jump_seq++;
					player_y = PLAYER_Y_BASE -6;
				}else if (jump_seq == 8){
					jump_seq++;
					player_y = PLAYER_Y_BASE -6;
				}else if (jump_seq == 9){
					jump_seq ++;
					player_y = PLAYER_Y_BASE -5;
				}else if (jump_seq == 10){
					jump_seq ++;
					player_y = PLAYER_Y_BASE -4;
				}else if (jump_seq == 11){
					jump_seq ++;
					player_y = PLAYER_Y_BASE -2;
				}else if (jump_seq == 12){
					jump_seq = 0;
					player_y = PLAYER_Y_BASE;
				}
				
				// If a thing is update each tick
				if (points >30){
					for (i = 0; i < 15; i++){
						if (game_bar[i].type != POINTS_BAR && game_bar[i].type!= UNUSED){
							if (game_bar[i].y_pos >= BAR_BUTTON){
								LCD_clear_bar(game_bar[i].lanes, game_bar[i].type);
								game_bar[i].type = UNUSED;
								
							}
							game_bar[i].y_pos += 1;
						}else	if (game_bar[i].type == POINTS_BAR){
							if (game_bar[i].y_pos >= POINTS_BUTTON){
								LCD_clear_bar(game_bar[i].lanes, game_bar[i].type);
								game_bar[i].type = UNUSED;
							}
							game_bar[i].y_pos += 2;
						}
						if (game_bar[i].type != UNUSED){
							LCD_draw_bar(game_bar[i].type,game_bar[i].lanes,game_bar[i].y_pos); 
						}
					}
				}
				
			}
			
			
			
		}
		if (game_started == 1){
			
			for (i = 0; i < 15; i++){
				if (check_collision(game_bar[i].type,game_bar[i].lanes,game_bar[i].y_pos,player_x,player_y)){
					if (game_bar[i].type == POINTS_BAR){
						points +=10;
					}else {
						game_over_flag = 1;
						game_started = 0;
						break;
					}
				}
			}
		}
		
		if (game_over_flag == 1){
			if (points > highest_score) {
				eeprom_write_score(points);
			}
			
			// clear and reprint game start message
			lcd_set_pos(0,COLS - 1, 293,307);
			for (i=293;i< 307 ;i++)
			{
						for(j= 0; j < COLS; j++)
						{
								lcd_write_data_u16(LCD_COLOR_BLACK);
						}
			}
			lcd_draw_image( 
									120,                 // X Pos
                  start_gameWidthPixels,   // Image Horizontal Width
                  300,                 // Y Pos
                  start_gameHeightPixels,  // Image Vertical Height
                  start_gameBitmaps,       // Image
                  LCD_COLOR_WHITE,      // Foreground Color
                  LCD_COLOR_BLACK     // Background Color
                );
			for(i = 0; i < 15; i++){
				game_bar[i].type = UNUSED;
			}
			game_over_flag = 0;

		}
		boundary_check(current_lane);	
		// Ready to next while loop
		LCD_draw_player(player_x, player_y);
		// clear all interrupt
		
		Button_real_flag = 0;
		read_button();
		
				
		tick = false;
		
		
		
		
	};
}
