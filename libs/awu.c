
#include "stm8s.h"

#include "awu.h"

//Unfortunately SDCC requires ISR to be in the file which contains main().
//void AWU_ISR(void) __interrupt(AWU_INT);  

void CLK_Config(void){
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); //Clock divider to HSI/1
}

void AWU_Config(void)
{
  //calibrate LSI for accurate auto wake up time base:
  AWU_LSICalibrationConfig(LSIMeasurement());

  //AWU only has a small number of predetermined sleep durations:
  //AWU_Init(AWU_TIMEBASE_2S);
  AWU_Init(AWU_TIMEBASE_12S);
}

uint32_t LSIMeasurement(void){
  
  uint32_t lsi_freq_hz = 0x0;
  uint32_t fmaster = 0x0;
  uint16_t ICValue1 = 0x0;
  uint16_t ICValue2 = 0x0;
  
  fmaster = CLK_GetClockFreq();  // Get master frequency
  
  // Enable the LSI measurement: LSI clock connected to timer Input Capture 1 
  AWU->CSR |= AWU_CSR_MSR;
  
  // Measure the LSI frequency with TIMER Input Capture 1 
  
  // Capture only every 8 events!!! 
  // Enable capture of TI1 
  TIM1_ICInit(TIM1_CHANNEL_1, TIM1_ICPOLARITY_RISING, TIM1_ICSELECTION_DIRECTTI, TIM1_ICPSC_DIV8, 0);
  
  TIM1_Cmd(ENABLE);
  
  // wait for a capture on cc1:
  while((TIM1->SR1 & TIM1_FLAG_CC1) != TIM1_FLAG_CC1);  
  
  ICValue1 = TIM1_GetCapture1();   // Get CCR1 value
  TIM1_ClearFlag(TIM1_FLAG_CC1);
  
  while((TIM1->SR1 & TIM1_FLAG_CC1) != TIM1_FLAG_CC1); // wait a capture on cc1
  ICValue2 = TIM1_GetCapture1();  // Get CCR1 value
  TIM1_ClearFlag(TIM1_FLAG_CC1);
  
  
  TIM1->CCER1 &= (uint8_t)(~TIM1_CCER1_CC1E);  // Disable IC1 input capture
  TIM1_Cmd(DISABLE);  // Disable timer
  
  
  // Compute LSI clock frequency
  lsi_freq_hz = (8 * fmaster) / (ICValue2 - ICValue1);
  
  // Disable the LSI measurement:
  //  LSI clock disconnected from timer Input Capture 1
  AWU->CSR &= (uint8_t)(~AWU_CSR_MSR);
  
  return (lsi_freq_hz);
}

/* 
//Unfortunately SDCC requires ISR to be in the file which contains main().
void AWU_ISR(void) __interrupt(AWU_INT)
//INTERRUPT_HANDLER(AWU_IRQHandler, AWU_INT)
{
  //Clear AWU peripheral pending bit
  AWU_GetFlagStatus();
}

*/
