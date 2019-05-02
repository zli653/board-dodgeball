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
void boundary_check() {
	 int play_x_left = player_x - playerWidthPixels/2;
   	 int play_x_right = player_x + playerWidthPixels/2;
	if (play_x_left/2 < 2) {
			player_x = playerWidthPixels/2+2; 
		}
		// 81
		else if (75 <= play_x_left && play_x_left <= 81) {
			player_x = 81+playerWidthPixels/2; 
		}	
		// 165
		else if (155 <= play_x_left && play_x_left <= 162) {
			player_x = 162+playerWidthPixels/2; 
		}	
		
		if (play_x_right > 240) {
			player_x = 240-playerWidthPixels/2;
		}
		else if (155 <= play_x_right && play_x_right <= 165) {
			player_x = 155-playerWidthPixels/2; 
		}
		else if (75 <= play_x_right && play_x_right <= 81) {
			player_x = 75-playerWidthPixels/2; 
		}
}

typedef enum 
{
  NOT_TOUCHED,
	NOT_TOUCHED_FIRST,
	NOT_TOUCHED_SECOND,
	IN_TOUCH,
	TOUCHED_FIRST,
	TOUCHED_SECOND
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
	if(gpio_enable_port(GPIOD_BASE) == false)   return false;
	if(gpio_enable_port(GPIOE_BASE) == false)   return false;
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
	// TODO: initial all hardware here
	DisableInterrupts();
	init_gpio();
	
	init_serial_debug(true, true);
	if(!init_i2c()) return false;

	lcd_config_screen();
	lcd_clear_screen(LCD_COLOR_BLACK);
	io_expander_init();
	
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

	
//*****************************************************************************
//*****************************************************************************
int 
main(void)
{ 
	bars *game_bar;
	bool jump;
	// char msg[80];
	int jump_count = 0;
	int8_t accel_flag;
	uint32_t i;
	uint8_t tick_count,tick_count_t, jump_seq,num_bars_exist,gen_bar_flag;
	bool tick;
	int16_t accel_x,  highest_score,points;

	
	init_hardware();
	eeprom_print_info();
	LCD_map_init();
	
	num_bars_exist = 0;
	gen_bar_flag = 0;
	jump = 0;	
	jump_seq = 0;
	tick = false;

	// LCD_draw_bar(LONG_BAR, 1, 202);
	eeprom_write_score(999);
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
		if ((Button_real_flag & BUTTON_SW2) != 0){
			eeprom_write_info();
			Button_real_flag &= BUTTON_SW2;
			
		}
		
		
		// If game has not started
		if (game_started == 0){
			if ((Button_real_flag & BUTTON_DOWN) != 0){
				game_started = 1;
				points = 0;
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
				for (i = 0; i <= 15; i++){
					if (i == 15) break;
					if (game_bar[i].type == UNUSED) break;
				}
				if (i != 15){
					game_bar[num_bars_exist].type = LONG_BAR;
					game_bar[num_bars_exist].y_pos = BAR_TOP;
					game_bar[num_bars_exist].lanes = rand() % 3;
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
			}
			
			if ((Button_real_flag & BUTTON_RIGHT) != 0){
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
				boundary_check();
				// If a thing is update each tick
				for (i = 0; i < 15; i++){
					if (game_bar[i].type != POINTS_BAR || game_bar[i].type!= UNUSED){
						if (game_bar[i].y_pos >= BAR_BUTTON){
							game_bar[i].type = UNUSED;
						}
						game_bar[i].y_pos += 1;
					}else	if (game_bar[i].type == POINTS_BAR){
						if (game_bar[i].y_pos >= POINTS_BUTTON){
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
			
		// Ready to next while loop
		LCD_draw_player(player_x, player_y);
		
		
				
		tick = false;
		
		
		
		
	};
}
