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
bool jump;
bool jumping;
uint16_t value;
uint16_t player_x;
uint16_t player_y;
int player_y_offset;
int player_y_step;
uint8_t current_lane;
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

typedef enum 
{
  NOT_TOUCHED,
	NOT_TOUCHED_FINAL,
	IN_TOUCH,
	TOUCHED_FINAL,
} TOUCH_STATES;

static TOUCH_STATES state = NOT_TOUCHED;
  
bool touch_fsm_sw(uint8_t touch_event)
{
	// uint8_t touch_event;
	// uint16_t touch_x, touch_y;
	switch (state)
	{
		case NOT_TOUCHED:
		{
			if(touch_event > 0) {state = TOUCHED_FINAL; }
			else { state = NOT_TOUCHED; }
		  return false;
			break;
		}
		case TOUCHED_FINAL:
		{
			if(touch_event > 0) {state = IN_TOUCH; return true;}
			else { state = NOT_TOUCHED; }
		  return false;
			break;
		}
		case NOT_TOUCHED_FINAL:
		{
			if(touch_event > 0) {state = IN_TOUCH;}
			else { state = NOT_TOUCHED; }
		  return false;
			break;
		}
		case IN_TOUCH:
		{
			if(touch_event > 0) {state = IN_TOUCH;}
			else { state = NOT_TOUCHED_FINAL; }
			return false;
			break;
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
	if ( count == 1)
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
	jump = 0;
	current_lane  = 1;
	// at the end of enable timer
	player_x = 120;
	player_y = PLAYER_Y_BASE;
	player_y_offset = 0;
  
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
	// char msg[80];
		int jump_count = 0;
	uint32_t i;
	uint8_t touch_event, tick_count;
	bool tick;
	int16_t accel_x, touch_x, touch_y, highest_score,points;
	value = 0;

	
		init_hardware();
	// eeprom print name
	eeprom_print_info();
	LCD_map_init();
	jump = 0;	
	jumping = false;
	tick = false;
	LCD_draw_bar(LEFT_BAR, 1, 50);
	LCD_draw_bar(RIGHT_BAR, 1, 150);
	LCD_draw_bar(RIGHT_BAR, 1, 50);
	LCD_draw_bar(RIGHT_BAR, 2, 50);
	LCD_draw_bar(LEFT_BAR, 0, 100);
	LCD_draw_bar(RIGHT_BAR, 0, 50);
	LCD_draw_bar(LEFT_BAR, 2, 100);
	lcd_draw_block(40, 60,150,60,LCD_COLOR_BROWN);
	eeprom_write_score(999);
	while(1){

		// Read input
		if(ALERT_TIMER1_LED_UPDATE) {
			// LED_blind();
			if (game_started){
				points++;
			}
			ALERT_TIMER1_LED_UPDATE = false;
    }
    if(ALERT_TIMER4_ACC_UPDATE) {
			// put_string("SEC :James");
			// check accelerator();
			accel_x = accel_read_x();	
			touch_event = ft6x06_read_td_status();
			jump = touch_fsm_sw(touch_event);
			// printf("ACCEL X: %d\n",accel_x);
			if (accel_x > 8000) {
				player_x -= 2;
			}	
			else if (accel_x > 2000) {
				player_x--;
			}
			else if (accel_x < -8000) {
				player_x += 2;
			}
			else if (accel_x < -2000) {
				player_x++;
			}	
			tick = true;
			ALERT_TIMER4_ACC_UPDATE = false;
    }
		
		touch_event = ft6x06_read_td_status();
//		if (touch_event == 1){
//			touch_x = ft6x06_read_x();
//			if (current_lane == 0){
//				if (touch_x > 100) touch_event = 0; 
//			}else if (current_lane == 1) {
//				if (touch_x > 180 || touch_x < 60 ) touch_event = 0; 
//			}else {
//				if (touch_x < 160) touch_event = 0; 
//			}
//		}
		// jump = touch_fsm_sw(touch_event);
		// if (!jumping) {jump = touch_fsm_sw(touch_event);}
			

		
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
		
		if (game_started == 0){
			if ((Button_real_flag & BUTTON_DOWN) != 0){
				game_started = 1;
				points = 0;
				highest_score = eeprom_print_score();
				LCD_score_init(highest_score);
				Button_real_flag &= ~BUTTON_DOWN;
			}
		} 
		if (game_started ==1){
			
			
			LCD_update_score(points);
			if (tick){
				tick_count = (tick_count +1)%2;
				if (tick_count == 0){
					// if update each 2 tick
				}
				// If a thing is update each tick
				if (jump != 0){
					jump_count++;
					printf("Jump times: %d\n\r", jump_count);
					jump = 0;
					player_y_step = 4;
					player_y_offset = 0;
					jumping = true;
				}
				if (player_y_step > -5){
					player_y_offset +=  player_y_step;
					player_y = PLAYER_Y_BASE - player_y_offset;
					player_y_step --;
				} else {
					jumping = false;
				}
			}
		}
			
		LCD_draw_player(player_x, player_y);
		
		
		// for(i = 0; i < 100000; i++){}
		tick = false;
	
	};
	// Reach infinite loop after the game is over.
}
