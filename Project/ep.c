#include "ep.h"


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
