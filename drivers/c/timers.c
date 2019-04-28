#include "timers.h"


//*****************************************************************************
// Verifies that the base address is a valid GPIO base address
//*****************************************************************************
static bool verify_base_addr(uint32_t base_addr)
{
   switch( base_addr )
   {
     case TIMER0_BASE:
     case TIMER1_BASE:
     case TIMER2_BASE:
     case TIMER3_BASE:
     case TIMER4_BASE:
     case TIMER5_BASE:
     {
       return true;
     }
     default:
     {
       return false;
     }
   }
}

//*****************************************************************************
// Returns the RCGC and PR masks for a given TIMER base address
//*****************************************************************************
static bool get_clock_masks(uint32_t base_addr, uint32_t *timer_rcgc_mask, uint32_t *timer_pr_mask)
{
  // Set the timer_rcgc_mask and timer_pr_mask using the appropriate
  // #defines in ../include/sysctrl.h
  switch(base_addr)
  {
    case TIMER0_BASE:
    {
      *timer_rcgc_mask = SYSCTL_RCGCTIMER_R0;
      *timer_pr_mask = SYSCTL_PRTIMER_R0;
      break;
    }
    case TIMER1_BASE:
    {
      *timer_rcgc_mask = SYSCTL_RCGCTIMER_R1;
      *timer_pr_mask = SYSCTL_PRTIMER_R1;
      break;
    }
    case TIMER2_BASE:
    {
      *timer_rcgc_mask = SYSCTL_RCGCTIMER_R2;
      *timer_pr_mask = SYSCTL_PRTIMER_R2;
      break;
    }
    case TIMER3_BASE:
    {
      *timer_rcgc_mask = SYSCTL_RCGCTIMER_R3;
      *timer_pr_mask = SYSCTL_PRTIMER_R3;
      break;
    }
    case TIMER4_BASE:
    {
      *timer_rcgc_mask = SYSCTL_RCGCTIMER_R4;
      *timer_pr_mask = SYSCTL_PRTIMER_R4;
      break;
    }
    case TIMER5_BASE:
    {
      *timer_rcgc_mask = SYSCTL_RCGCTIMER_R5;
      *timer_pr_mask = SYSCTL_PRTIMER_R5;
      break;
    }
    default:
    {
      return false;
    }
  }
  return true;
}


//*****************************************************************************
// Waits for 'ticks' number of clock cycles and then returns.
//
//The function returns true if the base_addr is a valid general purpose timer
//*****************************************************************************
bool gp_timer_wait(uint32_t base_addr, uint32_t ticks)
{
  TIMER0_Type *gp_timer;
  
  // Verify the base address.
  if ( ! verify_base_addr(base_addr) )
  {
    return false;
  }

  // Type cast the base address to a TIMER0_Type struct
  gp_timer = (TIMER0_Type *)base_addr;

  //*********************    
  // ADD CODE
  //*********************
	// turn the timer off	
  gp_timer->CTL &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);
	
  // sets the interval of Timer A to be the value of parameter 'ticks'
	gp_timer->TAILR = ticks;
	
	// acknowledges (clears) a Timer A timeout
	gp_timer->ICR |= TIMER_ICR_TATOCINT;
	
	gp_timer->CTL |= TIMER_CTL_TAEN;
	// busy-waits until a Timer A timeout occurs
	while( (gp_timer->RIS & TIMER_RIS_TATORIS) == 0) {}
	
  return true;
}


//*****************************************************************************
// Configure a general purpose timer to be a 32-bit timer.  
//
// Paramters
//  base_address          The base address of a general purpose timer
//
//  mode                  bit mask for Periodic, One-Shot, or Capture
//
//  count_up              When true, the timer counts up.  When false, it counts
//                        down
//
//  enable_interrupts     When set to true, the timer generates and interrupt
//                        when the timer expires.  When set to false, the timer
//                        does not generate interrupts.
//
//The function returns true if the base_addr is a valid general purpose timer
//*****************************************************************************
bool gp_timer_config_32(uint32_t base_addr, uint32_t mode, bool count_up, bool enable_interrupts)
{
  uint32_t timer_rcgc_mask;
  uint32_t timer_pr_mask;
  TIMER0_Type *gp_timer;
  
  // Verify the base address.
  if ( ! verify_base_addr(base_addr) )
  {
    return false;
  }
  
  // get the correct RCGC and PR masks for the base address
  get_clock_masks(base_addr, &timer_rcgc_mask, &timer_pr_mask);
  
  // Turn on the clock for the timer
  SYSCTL->RCGCTIMER |= timer_rcgc_mask;

  // Wait for the timer to turn on
  while( (SYSCTL->PRTIMER & timer_pr_mask) == 0) {};
  
  // Type cast the base address to a TIMER0_Type struct
  gp_timer = (TIMER0_Type *)base_addr;
    
  //*********************    
  // ADD CODE
  //*********************
	// turn the timer off	
  gp_timer->CTL &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);  
  
	// set the timer for 32-bit mode	
  gp_timer->CFG = TIMER_CFG_32_BIT_TIMER;	

	// clear the mode bits	
	gp_timer->TAMR &= ~TIMER_TAMR_TAMR_M;
	// set the timer's mode based on the 'mode' parameter
  gp_timer->TAMR |= mode;	

	// sets the direction of a General Purpose Timer based on the 'count_up' parameter
	gp_timer->TAMR |= (count_up & TIMER_TAMR_TACDIR);	

  // nables or disables Timer A timeout interrupts based on the 'enable_interrupts' parameter
	gp_timer->IMR |= (enable_interrupts & TIMER_IMR_TATOIM);
  return true;  
}

IRQn_Type timer_get_irq_num(uint32_t base)
{
   switch(base)
   {
     case TIMER1_BASE:
     {
       return TIMER1A_IRQn;
     }
     case TIMER4_BASE:
     {
       return TIMER4A_IRQn;
     }
     default:
     {
       return 0;
     }
   }
}

// enable timer A with specific ticks
bool gp_timer_enable(uint32_t base_addr, uint32_t ticks)
{
  TIMER0_Type *gp_timer;
  IRQn_Type interrupt_IRQ;
	
  // Verify the base address.
  if ( ! verify_base_addr(base_addr) )
  {
    return false;
  }

  // Type cast the base address to a TIMER0_Type struct
  gp_timer = (TIMER0_Type *)base_addr;

  	interrupt_IRQ = timer_get_irq_num(base_addr);
	// Set the Priority
  NVIC_SetPriority(interrupt_IRQ, 1);

  // Enable the Interrupt in the NVIC
  NVIC_EnableIRQ(interrupt_IRQ);
  
  //*********************    
  // ADD CODE
  //*********************
	// turn the timer off	
  gp_timer->CTL &= ~(TIMER_CTL_TAEN | TIMER_CTL_TBEN);
	
  // sets the interval of Timer A to be the value of parameter 'ticks'
	gp_timer->TAILR = ticks;
	
	
	
	gp_timer->CTL |= TIMER_CTL_TAEN;
	// busy-waits until a Timer A timeout occurs
	// while( (gp_timer->RIS & TIMER_RIS_TATORIS) == 0) {}
	
  return true;
}
