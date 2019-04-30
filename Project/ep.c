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
	for(addr = addr_start; addr <(addr_start + FIELD_BYTES); addr++)
	{
      	eeprom_byte_write(I2C1_BASE,addr, student1[addr - addr_start]);
	}
	addr_start = ADDR_START + FIELD_BYTES;
	for(addr = addr_start; addr <(addr_start + FIELD_BYTES); addr++)
	{
    	eeprom_byte_write(I2C1_BASE,addr, student2[addr - addr_start]);
	}
	addr_start = ADDR_START + 2 * FIELD_BYTES;	
	for(addr = addr_start; addr <(addr_start + FIELD_BYTES); addr++)
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
	for(addr = addr_start; addr <(addr_start + FIELD_BYTES); addr++)
	{
      	eeprom_byte_read(I2C1_BASE,addr, (uint8_t*)&read_val);
			  // eeprom_byte_read(I2C1_BASE,addr, &read_val);
      	printf("%c", read_val);
	}
	addr_start = ADDR_START + FIELD_BYTES;
	printf("Student2: ");
	for(addr = addr_start; addr <(addr_start + FIELD_BYTES); addr++)
	{
      	eeprom_byte_read(I2C1_BASE,addr, (uint8_t*)&read_val);
      	printf("%c", read_val);
	}
	addr_start = ADDR_START + 2 * FIELD_BYTES;	
	printf("Team number: ");
	for(addr = addr_start; addr <(addr_start + FIELD_BYTES); addr++)
	{
      	eeprom_byte_read(I2C1_BASE,addr, (uint8_t*)&read_val);
      	printf("%c", read_val);
	}
}

void eeprom_write_score(uint16_t score)
{
	uint16_t addr;
	uint16_t addr_start;
	uint8_t score_hi = score >> 8;
	uint8_t score_low = score & 0xF;

	addr_start = ADDR_START+NUM_FIELD*FIELD_BYTES;
	eeprom_byte_write(I2C1_BASE,addr, score_hi);
	eeprom_byte_write(I2C1_BASE,addr+8, score_low);
}

uint16_t eeprom_print_score(void)
{
	uint16_t addr_start;
	uint8_t xh, xl;
	addr_start = ADDR_START+NUM_FIELD*FIELD_BYTES;
	eeprom_byte_read(I2C1_BASE,addr_start, &xh);
	eeprom_byte_read(I2C1_BASE,addr_start+8, &xl);
	return (xh << 8 | xl); ;
}