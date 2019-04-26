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

//*****************************************************************************
//*****************************************************************************
void init_hardware(void)
{
	// TODO: initial all hardware here
	DisableInterrupts();
	init_serial_debug(true, true);
	eeprom_init();

	// i2c initialzation
	if(gpio_enable_port(GPIOA_BASE) == false)   return false;
	// Configure SCL 
	if(gpio_config_digital_enable(GPIOA_BASE, PA6)== false)  return false;
	if(gpio_config_digital_enable(GPIOA_BASE, PA7)== false)  return false;


	if(gpio_config_alternate_function(GPIOA_BASE, PA6)== false)  return false;
	if(gpio_config_alternate_function(GPIOA_BASE, PA7)== false)  return false;

	
	if(gpio_config_open_drain(GPIOA_BASE, PA7)== false)  return false;
	
	if(gpio_config_port_control(GPIOA_BASE, GPIO_PCTL_PA6_M, GPIO_PCTL_PA6_I2C1SCL)== false)  return false;
	if(gpio_config_port_control(GPIOA_BASE, GPIO_PCTL_PA7_M, GPIO_PCTL_PA7_I2C1SDA)== false)   return false;

	if(initializeI2CMaster(I2C1_BASE)!= I2C_OK)  return false;


	init_serial_debug(true, true);
	lcd_config_gpio();
	lcd_config_screen();
	lcd_clear_screen(LCD_COLOR_BLACK);  
	// TODO: check IO expander open gpio port A 
	// TODO: check overlapping open gpio port

	EnableInterrupts();

}

void eeprom_write_info(void)
{
	uint16_t addr;
	uint16_t addr_start;
	char student1[80] = "Zeming Li\n";
	char student2[80] = "Yan Xiao\n";
	// TODO: change team number
	char team_num[80] = "20\n";

	addr_start = ADDR_START;
	for(addr = addr_start; addr <(addr_start + NUM_BYTES); addr++)
	{
      	eeprom_byte_write(I2C1_BASE,addr, values[addr - addr_start]);
	}
	addr_start = ADDR_START + NUM_BYTES;
	for(addr = addr_start; addr <(addr_start + NUM_BYTES); addr++)
	{
    	eeprom_byte_write(I2C1_BASE,addr, values[addr - addr_start]);
	}
	addr_start = ADDR_START + 2 * NUM_BYTES;	
	for(addr = addr_start; addr <(addr_start + NUM_BYTES); addr++)
	{
      	eeprom_byte_write(I2C1_BASE,addr, values[addr - addr_start]);
	}

}

void eeprom_print_info(void){
	uint16_t addr;
	char values[80];
	char read_val[8];

	addr_start = ADDR_START;
	printf("Student1: ");
	for(addr = addr_start; addr <(addr_start + NUM_BYTES); addr++)
	{
      	eeprom_byte_read(I2C1_BASE,addr, &read_val);
      	printf("%s", read_val);
	}
	addr_start = ADDR_START + NUM_BYTES;
	printf("Student2: ");
	for(addr = addr_start; addr <(addr_start + NUM_BYTES); addr++)
	{
      	eeprom_byte_read(I2C1_BASE,addr, &read_val);
      	printf("%s", read_val);
	}
	addr_start = ADDR_START + 2 * NUM_BYTES;	
	printf("Team number: ");
	for(addr = addr_start; addr <(addr_start + NUM_BYTES); addr++)
	{
      	eeprom_byte_read(I2C1_BASE,addr, &read_val);
      	printf("%s", read_val);
	}
}

//*****************************************************************************
//*****************************************************************************
int 
main(void)
{ 
	init_hardware();

	eeprom_write_info();
	eeprom_print_info();
	// init_serial_debug(true,true);
	// io_expander_init();
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
	while(1){};
	// Reach infinite loop after the game is over.
}
