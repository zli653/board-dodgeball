// By 2019, Yan Xiao, Zeming LI


#include "io_expander.h"



bool io_expander_init(void){
	if(gpio_enable_port(IO_EXPANDER_GPIO_BASE) == false)
  {
    return false;
  }
	// Configure SCL 
  if(gpio_config_digital_enable(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PIN)== false)
  {
    return false;
  }
	
	if(gpio_config_digital_enable(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_alternate_function(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PIN)== false)
  {
    return false;
  }
	if(gpio_config_alternate_function(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
	
  if(gpio_config_port_control(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PCTL_M, IO_EXPANDER_I2C_SCL_PIN_PCTL)== false)
  {
    return false;
  }
	if(gpio_config_port_control(IO_EXPANDER_GPIO_BASE,IO_EXPANDER_I2C_SDA_PCTL_M, IO_EXPANDER_I2C_SDA_PIN_PCTL)== false)
  {
    return false;
  }
	
	if( initializeI2CMaster(IO_EXPANDER_I2C_BASE)!= I2C_OK)
  {
    return false;
  }
	
	// the part above should be written to one file
	//TODO: gpio B
	uint8_t temp;
	// the part above should be written to one file
	io_expander_write_reg(MCP23017_IODIRA_R,0x00);
	io_expander_write_reg(MCP23017_IODIRB_R,0x0f);
	
	io_expander_write_reg(MCP23017_GPPUB_R,0x0f);
	
	temp = io_expander_read_reg(MCP23017_IOCONB_R);
	temp &= ~0x06;
	io_expander_write_reg(MCP23017_IOCONB_R,temp);
	
	io_expander_write_reg(MCP23017_GPINTENB_R,0x0f);
	
	io_expander_read_reg(MCP23017_INTCAPB_R);
	io_expander_read_reg(MCP23017_GPIOB_R);
	
	// io_expander_write_reg(MCP23017_DEFVALB_R,0xff);
	io_expander_write_reg(MCP23017_INTCONB_R,0x00);

	return true;
}

void io_expander_write_reg(uint8_t reg, uint8_t data){
	
	i2c_status_t status;
  
  // Before doing anything, make sure the I2C device is idle
  while (I2CMasterBusy(IO_EXPANDER_I2C_BASE)) {};

	status = i2cSetSlaveAddr(IO_EXPANDER_I2C_BASE,MCP23017_DEV_ID,I2C_WRITE);
	if (status != I2C_OK) return;
		
	status = i2cSendByte(IO_EXPANDER_I2C_BASE, reg, I2C_MCS_START | I2C_MCS_RUN);
	if (status != I2C_OK) return;
		
  // get the data
  status = i2cSendByte(IO_EXPANDER_I2C_BASE, data, I2C_MCS_RUN |I2C_MCS_STOP);
  if (status != I2C_OK) return;
}


uint8_t io_expander_read_reg(uint8_t reg){
	
	i2c_status_t status;
	uint8_t data;
	
	while ( I2CMasterBusy(IO_EXPANDER_I2C_BASE)) {};
			
	status = i2cSetSlaveAddr(IO_EXPANDER_I2C_BASE, MCP23017_DEV_ID, I2C_WRITE);		
	if (status != I2C_OK) return status;			
			
	status = i2cSendByte(IO_EXPANDER_I2C_BASE,reg ,I2C_MCS_START | I2C_MCS_RUN);
	if (status != I2C_OK) return status;			
	
	status = i2cSetSlaveAddr(IO_EXPANDER_I2C_BASE, MCP23017_DEV_ID, I2C_READ);
	if (status != I2C_OK) return status;

  status = i2cGetByte(IO_EXPANDER_I2C_BASE, &data, I2C_MCS_START|I2C_MCS_RUN | I2C_MCS_STOP);
	if (status != I2C_OK) return status;		
		
	return data;
			
}


void light_control(uint8_t bitmap){
	io_expander_write_reg(MCP23017_GPIOA_R,bitmap);
}

uint8_t read_button(){
	uint8_t status = io_expander_read_reg(MCP23017_INTCAPB_R);
	if ((status & (1<<DIR_BTN_UP_PIN)) != 0){
		return 0;
	}
	if ((status & (1<<DIR_BTN_DOWN_PIN)) != 0){
		return 1;
	}
	if ((status & (1<<DIR_BTN_LEFT_PIN)) != 0){
		return 2;
	}
	if ((status & (1<<DIR_BTN_RIGHT_PIN)) != 0){
		return 3;
	}
	
	return status;
}

