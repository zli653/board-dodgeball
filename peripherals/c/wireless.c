#include "wireless.h"

//*****************************************************************************
// Public Functions
//*****************************************************************************

//*****************************************************************************
//*****************************************************************************
wireless_com_status_t 
wireless_send_32(
  bool      blockOnFull,
  bool      retry,
  uint32_t  data
)
{ 
  uint8_t status;

  if( spiVerifyBaseAddr(wirelessPinConfig.rf_base))
  {
    // Check the status of the device
    status = wireless_get_status();

    if( wireless_status_tx_full_asserted(status) && (blockOnFull == false))
    {
       return NRF24L01_TX_FIFO_FULL;
    }

    // Wait while the TX FIFO is not full 
    while(wireless_status_tx_full_asserted(status))
    {
        status = wireless_get_status();
    }

    do
    {
      // Put into Standby-1
      wireless_CE_low();

      // Set tx_mode
      wireless_start_tx_mode();

      // Flush any outstanding info in the TX FIFO
      wireless_flush_tx_fifo();

      // Send the data to the TX_PLD
      wireless_tx_data_payload(data);

      // Pulse CE for a 15uS
      wireless_CE_Pulse();

       status = wireless_wait_for_tx_ds();

       if( status == false)
       {
         wireless_clear_max_rt();

       }
       else
       {
          // Clear the tx_ds bit
          wireless_clear_tx_ds();
       }
    } while( status == false && retry == true);

     // Default back to receive mode
      wireless_start_rx_mode();

      // Enable Wireless transmission
      wireless_CE_high();

    if (status == true)
    {
      return NRF24L01_TX_SUCCESS;
    }
    else
    {
      return NRF24L01_TX_PCK_LOST;
    }
  }
  else
  {
    return NRF24L01_ERR;
  }

}

//*****************************************************************************
//*****************************************************************************
wireless_com_status_t
wireless_get_32(
  bool      blockOnEmpty,
  uint32_t  *data
)
{
  //uint8_t status;
  if( spiVerifyBaseAddr(wirelessPinConfig.rf_base))
  {

    if( wireless_rx_fifo_empty() == false)
    {
      // Read data from Rx FIFO
      wireless_rx_data_payload(data);

      // If Rx FIFO is empty, clear the RX_DR bit in the status
      // register
      if ( wireless_rx_fifo_empty() ==  true)
      {
         // Clear the RX_DR bit
          wireless_reg_write(NRF24L01_STATUS_R, NRF24L01_STATUS_RX_DR_M);
      }

      return NRF24L01_RX_SUCCESS;
    }
    else if ( (wireless_rx_fifo_empty() == true) && blockOnEmpty)
    {

     // Wait until the RX_DR bit is set
      wireless_wait_for_rx_dr();

      // Read data from Rx FIFO
      wireless_rx_data_payload( data);

      // If Rx FIFO is empty, clear the RX_DR bit in the status
      // register
      if ( wireless_rx_fifo_empty() ==  true)
      {
         // Clear the RX_DR bit
          wireless_reg_write(NRF24L01_STATUS_R, NRF24L01_STATUS_RX_DR_M);
      }

      return NRF24L01_RX_SUCCESS;
    }
    else
    {
      return NRF24L01_RX_FIFO_EMPTY;
    }
  }
  else
  {
    return NRF24L01_ERR;
  }
}

//*****************************************************************************
//*****************************************************************************
bool wireless_configure_device( 
  uint8_t           *my_id,
  uint8_t           *dest_id
)
{

  if( spiVerifyBaseAddr(wirelessPinConfig.rf_base))
  {
    wireless_CSN_high();
    wireless_CE_low();

    // Configure Common RF settings
    wireless_flush_tx_fifo();
    wireless_flush_rx_fifo();
    wireless_reg_write(NRF24L01_RF_SETUP_R, NRF24L01_RF_SETUP_RF_PWR_0DB | NRF24L01_RF_SETUP_250_KBPS);
    wireless_reg_write(NRF24L01_RF_CH_R, wirelessPinConfig.channel);
    wireless_reg_write( NRF24L01_STATUS_R, NRF24L01_STATUS_CLEAR_ALL);
    wireless_reg_write(NRF24L01_SETUP_RETR_R, NRF24L01_SETUP_RETR_ARD_0750_US | NRF24L01_SETUP_RETR_ARC_15);

    // Configure the address to transfer data to
    wireless_set_tx_addr(dest_id);

    // Configure Pipe 0 to receive the AUTO ACKs from the other device
    wireless_reg_write(NRF24L01_RX_PW_P0_R, wirelessPinConfig.payload_size);
    wireless_set_rx_addr(dest_id, 0);

    // Configure Pipe 1
    wireless_reg_write(NRF24L01_RX_PW_P1_R, wirelessPinConfig.payload_size);
    wireless_set_rx_addr(my_id, 1);

    // Turn on Rx and AutoAcks for pipe 0 and 1
    wireless_reg_write(NRF24L01_EN_RXADDR_R, NRF24L01_RXADDR_ERX_P0 | NRF24L01_RXADDR_ERX_P1); 
    wireless_reg_write(NRF24L01_EN_AA_R, NRF24L01_ENAA_P0 | NRF24L01_ENAA_P1);

    // Enable the Radio in RX mode
    wireless_reg_write(NRF24L01_CONFIG_R,NRF24L01_CONFIG_PWR_UP | NRF24L01_CONFIG_EN_CRC | NRF24L01_CONFIG_PRIM_RX_PRX );

    wireless_CE_high();
    return true;
  }
  else
  {
    return false ;
  }
}

void wireless_initialize(void){
int i = 0;
	spi_select_init();
	
  gpio_enable_port(RF_GPIO_BASE);
  
  // Configure SPI CLK
  gpio_config_digital_enable(RF_GPIO_BASE, RF_CLK_PIN);
  gpio_config_alternate_function(RF_GPIO_BASE, RF_CLK_PIN);
  gpio_config_port_control(RF_GPIO_BASE, RF_SPI_CLK_PCTL_M, RF_CLK_PIN_PCTL);
  
  // Configure SPI MISO
  gpio_config_digital_enable(RF_GPIO_BASE, RF_MISO_PIN);
  gpio_config_alternate_function(RF_GPIO_BASE, RF_MISO_PIN);
  gpio_config_port_control(RF_GPIO_BASE, RF_SPI_MISO_PCTL_M, RF_MISO_PIN_PCTL);
  
  // Configure SPI MOSI
  gpio_config_digital_enable(RF_GPIO_BASE, RF_MOSI_PIN);
  gpio_config_alternate_function(RF_GPIO_BASE, RF_MOSI_PIN);
  gpio_config_port_control(RF_GPIO_BASE, RF_SPI_MOSI_PCTL_M, RF_MOSI_PIN_PCTL);
  
  // Configure CS to be a normal GPIO pin that is controlled 
  // explicitly by software
  gpio_enable_port(RF_CS_BASE);
  gpio_config_digital_enable(RF_CS_BASE,RF_CS_PIN);
  gpio_config_enable_output(RF_CS_BASE,RF_CS_PIN);

  initialize_spi(RF_SPI_BASE, RF_SPI_MODE, 10);
	
	while( accel_reg_read(ACCEL_WHO_AM_I_R) != 0x69)
	{
			for(i=0; i < 1000; i++)
			{};
	}


}