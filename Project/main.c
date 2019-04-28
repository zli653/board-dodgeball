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

volatile int Button_interrupted;
volatile int Button_value;
volatile int Button_value_old;

bool up_button;
bool right_button;
bool left_button;
bool down_button;
bool sw2_button;
int button_flag;


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

void TIMER1A_Handler(void)
{
	TIMER0_Type *gp_timer = (TIMER0_Type *)TIMER1_BASE;
	// acknowledges (clears) a Timer A timeout
	gp_timer->ICR |= TIMER_ICR_TATOCINT;
	
	ALERT_TIMER1_LED_UPDATE = true;
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
	init_serial_debug(true, true);
	init_gpio();
	// eeprom_init();
	if(!init_i2c()) return false;

	// lcd_config_gpio();
	lcd_config_screen();
	lcd_clear_screen(LCD_COLOR_BLACK);  
	io_expander_init();
	
	
  // inside initalize hardware
  // Initialize the TIMER1 to be a 
  //      32-bit
  //      Periodic
  //      count up
  //      generate interrupts 
  gp_timer_config_32(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, true, true);

	// init globals
	
	Button_interrupted = 0;
	Button_value = 0;
	Button_value_old = read_button();
	
	up_button = false;
  right_button = false;
  left_button = false;
  down_button = false;
  sw2_button = false;
	button_flag = 0;
	
	// at the end of enable timer
  gp_timer_enable(TIMER1_BASE, SEC_ONE);
  
	EnableInterrupts();
  return true;
}

void eeprom_write_info(void)
{
	uint16_t addr;
	uint16_t addr_start;
	unsigned char student1[80] = "Zeming Li\n";
	unsigned char student2[80] = "Yan Xiao\n";
	// TODO: change team number
	unsigned char team_num[80] = "20\n";

	addr_start = ADDR_START;
	for(addr = addr_start; addr <(addr_start + NUM_BYTES); addr++)
	{
      	eeprom_byte_write(I2C1_BASE,addr, student1[addr - addr_start]);
	}
	addr_start = ADDR_START + NUM_BYTES;
	for(addr = addr_start; addr <(addr_start + NUM_BYTES); addr++)
	{
    	eeprom_byte_write(I2C1_BASE,addr, student2[addr - addr_start]);
	}
	addr_start = ADDR_START + 2 * NUM_BYTES;	
	for(addr = addr_start; addr <(addr_start + NUM_BYTES); addr++)
	{
      	eeprom_byte_write(I2C1_BASE,addr, team_num[addr - addr_start]);
	}

}

void eeprom_print_info(void){
	uint16_t addr;
	uint16_t addr_start;
	unsigned char read_val;
	
	addr_start = ADDR_START;
	printf("Student1: ");
	for(addr = addr_start; addr <(addr_start + NUM_BYTES); addr++)
	{
      	eeprom_byte_read(I2C1_BASE,addr, (uint8_t*)&read_val);
			  // eeprom_byte_read(I2C1_BASE,addr, &read_val);
      	printf("%c", read_val);
	}
	addr_start = ADDR_START + NUM_BYTES;
	printf("Student2: ");
	for(addr = addr_start; addr <(addr_start + NUM_BYTES); addr++)
	{
      	eeprom_byte_read(I2C1_BASE,addr, (uint8_t*)&read_val);
      	printf("%c", read_val);
	}
	addr_start = ADDR_START + 2 * NUM_BYTES;	
	printf("Team number: ");
	for(addr = addr_start; addr <(addr_start + NUM_BYTES); addr++)
	{
      	eeprom_byte_read(I2C1_BASE,addr, (uint8_t*)&read_val);
      	printf("%c", read_val);
	}
}


void GPIOF_Handler(void){
	Button_interrupted = 1;
	GPIOF->ICR |= 0xff;
	Button_value = read_button();
	
}
//*****************************************************************************
//*****************************************************************************
int 
main(void)
{ 
	  char msg[80];
	init_hardware();
	
	// eeprom print name
	eeprom_print_info();
	
	// init_serial_debug(true,true);
	// io_expander_init();
		// reset
	// if SW2 is pressed, write name
	eeprom_write_info();
	
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
	while(1){
		if(ALERT_TIMER1_LED_UPDATE) {
			sprintf(msg,"SEC :James");
			// LED_blind();
			ALERT_TIMER1_LED_UPDATE = false;
    }
		
		if (Button_interrupted == 1){
			DisableInterrupts();
			Button_interrupted = 0;
			if (((Button_value & (1 << DIR_BTN_UP_PIN)) == 0) && ((Button_value_old & (1 << DIR_BTN_UP_PIN)) != 0)){
				light_control(0x1);
				up_button = true;
			}
			else if (((Button_value & (1 << DIR_BTN_DOWN_PIN)) == 0) && ((Button_value_old & (1 << DIR_BTN_DOWN_PIN)) != 0)){
				light_control(0x2);
				down_button = true;
			}
			else if (((Button_value & (1 << DIR_BTN_LEFT_PIN)) == 0) && ((Button_value_old & (1 << DIR_BTN_LEFT_PIN)) != 0)){
				light_control(0x4);
				left_button = true;
			}
			else if (((Button_value & (1 << DIR_BTN_RIGHT_PIN)) == 0) && ((Button_value_old & (1 << DIR_BTN_RIGHT_PIN)) != 0)){
				light_control(0x8);
				right_button = true;
			}
			else if (Button_value == Button_value_old ){
				light_control(0x10);
				sw2_button = true;
			}
			button_flag = (Button_value_old ^ Button_value) & (~Button_value);
			if (Button_value_old == Button_value) button_flag = 0x10;
			Button_value_old = Button_value;
			EnableInterrupts();
		}
		
		//if (button_flag != 0) light_control(button_flag);
//		if (up_button){
//			light_control(0x1);
//			up_button = false;
//		}
//		if (down_button){
//			light_control(0x2);
//			down_button = false;
//		}
//		if (left_button){
//			light_control(0x4);
//			left_button = false;
//		}
//		if (right_button){
//			light_control(0x8);
//			right_button = false;
//		}
//		if (sw2_button){
//			light_control(0x10);
//			sw2_button = false;
//		}
	
	};
	// Reach infinite loop after the game is over.
}
