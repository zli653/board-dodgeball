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

volatile int Button_interrupted;
volatile int Button_value;
int Button_old;
volatile int Button_flag;
int Button_real_flag;

int value;
int pick_plant[3];
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

void TIMER4A_Handler(void)
{
	  static int count = 0;

	TIMER0_Type *gp_timer = (TIMER0_Type *)TIMER4_BASE;
	// acknowledges (clears) a Timer A timeout
	gp_timer->ICR |= TIMER_ICR_TATOCINT;
	
	ALERT_TIMER4_ACC_UPDATE = true;
	/* if ( count == 125)
	{
			ALERT_TIMER4_ACC_UPDATE = true;
			count = 0;
	}
	else
	{
		count++;
	} */
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
	prepare_array();
	
	accel_initialize();
	
  // inside initalize hardware
  // Initialize the TIMER1 to be a 
  //      32-bit
  //      Periodic
  //      count up
  //      generate interrupts 
  gp_timer_config_32(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, true, true);

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
	Button_flag = read_interrupt();
	Button_value = read_button();
	Button_interrupted =	1;
	GPIOF->ICR |= 0xff;
	
}

void LCD_draw_map(){
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
	
}

void update_score(uint16_t score){
	uint8_t first = score % 10;
	uint8_t second = (score / 10) % 10;
	uint8_t last = (score / 100) % 10;
	// need to remove this part
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
									numbers[first],       // Image
									LCD_COLOR_WHITE,      // Foreground Color
									LCD_COLOR_BLACK     // Background Color
								);
		lcd_draw_image( 
									95,                 // X Pos
									numbers_WidthPixels,   // Image Horizontal Width
									300,                 // Y Pos
									numbers_HeightPixels,  // Image Vertical Height
									numbers[second],       // Image
									LCD_COLOR_WHITE,      // Foreground Color
									LCD_COLOR_BLACK     // Background Color
								);
		lcd_draw_image( 
									111,                 // X Pos
									numbers_WidthPixels,   // Image Horizontal Width
									300,                 // Y Pos
									numbers_HeightPixels,  // Image Vertical Height
									numbers[last],       // Image
									LCD_COLOR_WHITE,      // Foreground Color
									LCD_COLOR_BLACK     // Background Color
								);
		


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
//*****************************************************************************
//*****************************************************************************
int 
main(void)
{ 
	// char msg[80];
	uint8_t touch_event;
	int16_t accel_x, touch_x, touch_y;
	init_hardware();
	
	// eeprom print name
	eeprom_print_info();
	LCD_draw_map();
	update_score(100);
	// init_serial_debug(true,true);
	// io_expander_init();
		// reset
	
	
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
			printf("SEC :James");
			// LED_blind();
			ALERT_TIMER1_LED_UPDATE = false;
    }
    if(ALERT_TIMER4_ACC_UPDATE) {
			// put_string("SEC :James");
			// check accelerator();
			// accel_x = accel_read_x();	
			// printf("ACCEL X: %d\n",accel_x);
			ALERT_TIMER4_ACC_UPDATE = false;
    }
		touch_event = ft6x06_read_td_status();
		if (touch_event > 0) {
			touch_x = ft6x06_read_x();
			touch_y = ft6x06_read_y();
			printf("TOUCH X: %d\n",touch_x);
		}	
		if (Button_interrupted == 1){
			if (Button_flag == 0){
				Button_real_flag |= 0x10;
			} else {
				Button_real_flag = (Button_old) & (~Button_value);
				Button_old = Button_value;
			}
			Button_interrupted = 0;
		}
		
		
		
		
		if ((Button_real_flag & BUTTON_DOWN) != 0){
			value++;
			update_score(value);
			Button_real_flag &= ~BUTTON_DOWN;
		}
		
		// if SW2 is pressed, write name
		if ((Button_real_flag & BUTTON_SW2) != 0){
			eeprom_write_info();
			Button_real_flag &= BUTTON_SW2;
			
		}
		
		
	
	};
	// Reach infinite loop after the game is over.
}
