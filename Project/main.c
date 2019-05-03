// Student 1: Yan Xiao
// Student 2: Zeming Li
// Team number: 32


#include "main.h"

#define SEC_ONE     50000000
volatile bool ALERT_TIMER1_LED_UPDATE;
#define MSEC_EIGHT     50000
volatile bool ALERT_TIMER4_ACC_UPDATE;

volatile uint8_t Button_interrupted;
volatile uint8_t Button_value;
volatile uint8_t Button_flag;

uint8_t game_started;

uint16_t player_x;
uint16_t player_y;
uint8_t current_lane;

uint8_t masterID[] = {4,9,7,2,4};
uint8_t slaveID[] = {8,9,6,2,7};

uint16_t touch_x, touch_y;
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

// This helper function is used to make sure the player is in the lane,
// if not, move the player back
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
}





static TOUCH_STATES state = NOT_TOUCHED;
// This function is a FSM to reduce the posibility of wrong input
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
	}
}

// This is will step up timer flag and clear interrupt
void TIMER1A_Handler(void)
{
	TIMER0_Type *gp_timer = (TIMER0_Type *)TIMER1_BASE;
	// acknowledges (clears) a Timer A timeout
	gp_timer->ICR |= TIMER_ICR_TATOCINT;
	
	ALERT_TIMER1_LED_UPDATE = true;
}

// This is will step up timer flag and clear interrupt
void TIMER4A_Handler(void)
{

	TIMER0_Type *gp_timer = (TIMER0_Type *)TIMER4_BASE;
	// acknowledges (clears) a Timer A timeout
	gp_timer->ICR |= TIMER_ICR_TATOCINT;
	
	//ALERT_TIMER4_ACC_UPDATE = true;
	ALERT_TIMER4_ACC_UPDATE = true;

}	

// init all gpio ports to prevent double init
bool init_gpio(){
	if(gpio_enable_port(GPIOA_BASE) == false)   return false;
	if(gpio_enable_port(GPIOB_BASE) == false)   return false;
	if(gpio_enable_port(GPIOC_BASE) == false)   return false;
	if(gpio_enable_port(GPIOD_BASE) == false)   return false;
	if(gpio_enable_port(GPIOF_BASE) == false)   return false;
	return true;
}

// init i2c interface to all i2c devices
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
	

  gp_timer_config_32(TIMER1_BASE, TIMER_TAMR_TAMR_PERIOD, true, true);
	gp_timer_enable(TIMER1_BASE, SEC_ONE);

  gp_timer_config_16(TIMER4_BASE, TIMER_TAMR_TAMR_PERIOD, false, true, 7); 
  gp_timer_enable(TIMER4_BASE, MSEC_EIGHT);

	// init globals
	
	Button_interrupted = 0;
	Button_value = 0;
	Button_flag = 0x0;

	game_started = 0;
	current_lane  = 1;

	player_x = 120;
	player_y = PLAYER_Y_BASE;
  
	spi_select_init();
	
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
	uint8_t gen_bar_flag,gen_bar_count; //in game flag 
	
	bool jump;				//in game flag 
	uint8_t jump_seq;
	
	int8_t accel_flag; //in game flag 
	int16_t accel_x;
	
	uint32_t i,j;
	uint8_t tick_count,tick_count_t;
	bool tick;	//in game flag 
	
	int16_t highest_score, points;
	
	uint8_t Button_old, Button_real_flag;
	
	uint8_t game_over_flag;
	uint8_t game_over_count;
	
	uint8_t wireless_flag;
	uint8_t wireless_lane = 3;
	wireless_com_status_t wireless_status;
	uint32_t wireless_data;
	// true for slave (one player)
	bool master_slave = true; // Change this for master device true of player, false for others

	uint8_t wireless_master_flag = 0;
	uint8_t wireless_slave_flag = 0;
		wireless_flag = 0;
	game_over_flag = 0;
	game_over_count = 0;
	
	gen_bar_flag = 0;
	gen_bar_count = 0;
	game_bar = malloc(sizeof(bars)*15);
	for(i = 0; i < 15; i++){
		game_bar[i].type = UNUSED;
	}
	
	jump = 0;	
	jump_seq = 0;
	
	tick = false;
	
	Button_old = 0x0f;
	Button_real_flag = 0x00;

	if (!init_hardware()){
		while(1){};
	}
	
	eeprom_print_info();
	LCD_map_init();
	if (master_slave){
			spi_select(NORDIC);
			wireless_configure_device(slaveID, masterID);
			wireless_slave_flag = 1;
			wireless_flag = 1;
		}
	if (!master_slave){
		spi_select(NORDIC);
		wireless_configure_device(masterID, slaveID);
		wireless_flag = 1;
		wireless_master_flag = 1;
	}
	
	while(1){
		//// Gather input area
		
		/// Timer
		// 1s timer
		if(ALERT_TIMER1_LED_UPDATE) {
			light_blink();
			if (game_started){
				points++;
				gen_bar_flag = 1;
			}
			if (game_over_count >0){
				game_over_count --;
			}
			read_button();
			ALERT_TIMER1_LED_UPDATE = false;
    }
		
		// 8ms timer
    if(ALERT_TIMER4_ACC_UPDATE) {
			// if game has started
			if (game_started == 1){
				spi_select(MODULE_1);
				accel_x = accel_read_x();	
				jump = touch_fsm_sw(current_lane);
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
				tick = true;
			}
			ALERT_TIMER4_ACC_UPDATE = false;
    }
	
		
		/// Handle button interrupt
		if (Button_interrupted == 1){
			if (Button_flag == 0){
				Button_real_flag |= 0x10;
			} else {
				Button_real_flag = (Button_old) & (~Button_value);
				Button_old = Button_value;
			}
			Button_interrupted = 0;
		}
		
		// if this is master, send the button information to slave
		if (wireless_flag == 1 && wireless_master_flag == 1){
			if ((Button_real_flag & BUTTON_LEFT) != 0){
				spi_select(NORDIC);
				wireless_status = wireless_send_32(false,true,1);
				while(wireless_status != NRF24L01_TX_SUCCESS){
					wireless_status = wireless_send_32(false,false,1);
				}
				Button_real_flag &= ~BUTTON_LEFT;
			}
			else if ((Button_real_flag & BUTTON_RIGHT) != 0 && wireless_flag == 1){
				spi_select(NORDIC);
				wireless_status = wireless_send_32(false,true,3);
				while(wireless_status != NRF24L01_TX_SUCCESS){
					wireless_status = wireless_send_32(false,false,3);
				}
				Button_real_flag &= ~BUTTON_RIGHT;
			}
			else if ((Button_real_flag & BUTTON_DOWN) != 0 && wireless_flag == 1){
				spi_select(NORDIC);
				wireless_status = wireless_send_32(false,true,2);
				while(wireless_status != NRF24L01_TX_SUCCESS){
				  wireless_status = wireless_send_32(false,false,2);
				}
				Button_real_flag &= ~BUTTON_DOWN;
			}
		}
		
		// restart the game if game over
		if (game_over_count == 1){
			lcd_clear_screen(LCD_COLOR_BLACK);
			LCD_map_init();
			game_started = 0;
			game_over_count = 0;
		}
		
		// If game has not started
		if (game_started == 0 && game_over_count == 0){
			if ((Button_real_flag & BUTTON_SW2) != 0){
				eeprom_write_info();
				Button_real_flag &= BUTTON_SW2;
			}
			// main player restart game if down button is pressed
			if (!(wireless_flag == 1 && wireless_master_flag == 1)){
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
		
		} 
		
		// main player get sent data
		if (wireless_flag == 1 && wireless_slave_flag == 1){
				spi_select(NORDIC);
				wireless_status = wireless_get_32(false, &wireless_data);
				if (wireless_status ==  NRF24L01_RX_SUCCESS){
					if (wireless_data != 0){
						wireless_lane = wireless_data -1;
					}
				}
		}
		
		// If game has started
		if (game_started ==1 && game_over_count == 0 ){
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
					jump = 0; // clearing jump flag
				}
				
				// generate bar if get new data from wireless 
				if (wireless_lane != 3){
					for (i = 0; i <= 15; i++){
						if (i == 15) break;
						if (game_bar[i].type == UNUSED) break;
					}
					if (i != 15){
						game_bar[i].type = rand() % 2+2;
						game_bar[i].y_pos = BAR_TOP;
						game_bar[i].lanes = wireless_lane;
						game_bar[i].points_hit =0;
					}
					// clear data
					wireless_lane = 3;
				}
				
				
				// generate bars each tick with different difficulties
				if (gen_bar_flag == 1){
					gen_bar_flag = 0; // clearing gen bar flag
					// highest speed
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
							game_bar[i].points_hit =0;
						}
						for (i = 0; i <= 15; i++){
							if (i == 15) break;
							if (game_bar[i].type == UNUSED) break;
						}
						if (i != 15){
							game_bar[i].type = rand() % 4+1;
							game_bar[i].y_pos = BAR_TOP;
							game_bar[i].lanes = (j+2)%3;
							game_bar[i].points_hit =0;
						}
					}
					// 60 <= points < 100
					else if (points >= 60){
						for (i = 0; i <= 15; i++){
							if (i == 15) break;
							if (game_bar[i].type == UNUSED) break;
						}
						if (i != 15){
							game_bar[i].type = rand() % 4+1;
							game_bar[i].y_pos = BAR_TOP;
							game_bar[i].lanes = rand() % 3;
							game_bar[i].points_hit =0;
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
								game_bar[i].points_hit =0;
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

				// undate image based on each tick. Make sure each the image update in each tick not constantly
				if (tick){
					tick_count = (tick_count +1)%2; // movement update once per 2 tick
					tick_count_t = (tick_count_t +1)%3; // movement update once per 3 tick
					
					// movement update once per 2 tick
					if (tick_count == 0){	
						// update jump animation
						if (jump_seq == 5 || jump_seq == 7){
							jump_seq++;
							player_y = PLAYER_Y_BASE -7;
						}
						
						// update left right movement
						if (accel_flag == 1 || accel_flag == -1){
							player_x --;
						}
						
						// update bars if score is less than 30
						if (points <= 30){
							for (i = 0; i < 15; i++){
								if (game_bar[i].type != POINTS_BAR && game_bar[i].type!= UNUSED){
									if (game_bar[i].y_pos >= BAR_BOTTOM){
										LCD_clear_bar(game_bar[i].lanes, game_bar[i].type);
										game_bar[i].type = UNUSED;
									}
									game_bar[i].y_pos += 1;
								} else if (game_bar[i].type == POINTS_BAR){
									if (game_bar[i].y_pos >= POINTS_BOTTOM){
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
					
					
					// update every 3 ticks
					if (tick_count_t == 0){
						if (jump_seq == 6){
							jump_seq++;
							player_y = PLAYER_Y_BASE - 8;
						}
					}
					
					// update each tick
					// left right
					if (accel_flag == 2 ||accel_flag == 3 ){
							player_x = player_x +accel_flag -1;
					}
					if (accel_flag == -2|| accel_flag == -3){
							player_x = player_x +accel_flag +1;
					}
					
					// jump sequence animation
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
					
					// If a thing is update  droping bars
					if (points > 30){
						for (i = 0; i < 15; i++){
							if (game_bar[i].type != POINTS_BAR && game_bar[i].type!= UNUSED){
								if (game_bar[i].y_pos >= BAR_BOTTOM){
									LCD_clear_bar(game_bar[i].lanes, game_bar[i].type);
									game_bar[i].type = UNUSED;
									
								}
								game_bar[i].y_pos += 1;
							}else	if (game_bar[i].type == POINTS_BAR){
								if (game_bar[i].y_pos >= POINTS_BOTTOM){
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
					
					// update score
					LCD_update_score(points);
					
					// the information is fully updated, start drawing the player based on the information
					boundary_check(current_lane);	
					// Ready to next while loop
					LCD_draw_player(player_x, player_y);
				}
				
				// detele collision between player and bars
				for (i = 0; i < 15; i++){
					if (check_collision(game_bar[i].type,game_bar[i].lanes,game_bar[i].y_pos,player_x,player_y)){
						if (game_bar[i].type == POINTS_BAR ){
							if ( game_bar[i].points_hit == 0){
								points +=10;
								game_bar[i].points_hit = 1;
							}
						}else {
							game_over_flag = 1;

							break;
						}
					}
				}
				
		}
		
		// if it failed the collision detection and game has end
		if (game_over_flag == 1){
			if (points > highest_score) {
				eeprom_write_score(points);
			}
			
			for(i = 0; i < 15; i++){
				game_bar[i].type = UNUSED;
			}
			game_over_flag = 0;
			game_over_count = 4;
		}
		
		// clear all flags, make sure it is cleaned
		Button_real_flag = 0;
		tick = false;
		jump = false;
		accel_flag = 0;
		gen_bar_flag  = 0;
	};
}
