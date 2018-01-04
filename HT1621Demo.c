
//Ahmet Onat

#include "stm8s.h"
#include "stm8s_adc1.h"
#include "awu.h"
#include "HT1621.h"


#define LED_PORT  GPIOB
#define LED_PIN   GPIO_PIN_5

#define LED_OFF() GPIO_WriteHigh(GPIOB, GPIO_PIN_5);
#define LED_ON()  GPIO_WriteLow(GPIOB, GPIO_PIN_5);
#define LED_TOGGLE() GPIO_WriteReverse(LED_PORT, LED_PIN);


// Pin connections: See HT1621.h file.

void AWU_ISR(void) __interrupt(AWU_INT); //Sleeps between screen refreshes.

void SetupADC();

void main(void) {

  uint16_t adc_res; //The latest ADC result is stored here.
  HT1621Values Seg_LCD; //Caution! No variable definitions after first instruction.
  
  GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_LOW_FAST); //Init LED.
  LED_OFF();
  
  CLK_Config(); //Initialize the clock.
  AWU_Config(); //Initilize the AWU peripheral.
  enableInterrupts();

  SetupADC();

  HT1621_PortInit();  //Processor port settings for the display.
  HT1621_Init();       //Init the display.
  HT1621_ValuesConstructor(&Seg_LCD); //Initialize LCD character (digit) map.

  HT1621_AllOn(16); //Turn on all segments.
  AWU_Init(AWU_TIMEBASE_1S); //Set alarm time to 1 second later.
  halt(); //Sleep.

  HT1621_AllOff(16); //Turn off all segments.
  AWU_Init(AWU_TIMEBASE_1S); //Set alarm time to 1 second later.
  halt(); //Sleep

  
  ADC1_Cmd(ENABLE); //ADC_CR1_ADON = 1;
  
  
  while(1){ //This is an ADC conversion demo.
    // See commented out code below for some other examples.
    
    ADC1_StartConversion(); //TODO: Check why twice ADON is necessary.
    ADC1_StartConversion(); //
    
    while(ADC1_GetFlagStatus(ADC1_FLAG_EOC)==0); //Wait until end of conversion.
    
    adc_res=ADC1_GetConversionValue(); //Read the result.
    ADC1_ClearFlag(ADC1_FLAG_EOC); //This does not seem to be necessary...
    
    
    Seg_LCD.decimalPt=4; //Equivalent to hiding the point. See HT1621.c

    //Do the ritual to convert data to LCD segment display:
    HT1621_Convert(adc_res,&Seg_LCD);
    HT1621_Blanking(&Seg_LCD);  //Remove leading zeros.
    // Write the data prepared above, at once.
    HT1621_Refresh (&Seg_LCD);

    //LED_TOGGLE();  //Aid in measuring timing.
    AWU_Init(AWU_TIMEBASE_128MS); //Repeat after sleeping 128ms.
    halt();
    
  }


  //These are some examples of how to manipulate some LCD segments directly:
  
  //Prepare the data to be written to the LCD:
  /*    
  Seg_LCD.decimalPt=1;
  HT1621_Refresh(&Seg_LCD);//turn on the first decimal point from the right
  Seg_LCD.digit[5]=Seg_LCD.map[5]; //Most significant digit.
  Seg_LCD.digit[4]=Seg_LCD.map[4];
  Seg_LCD.digit[3]=Seg_LCD.map[3];
  Seg_LCD.digit[2]=Seg_LCD.map[2];
  Seg_LCD.digit[1]=Seg_LCD.map[1];
  Seg_LCD.digit[0]=Seg_LCD.map[0]; //Least stignificant digit.
  
  sbi(Seg_LCD.digit[5],7);   //Set battery top tick.
  //cbi(Seg_LCD.digit[5],7);           //or clear
  sbi(Seg_LCD.digit[4],7);   //Set battery second tick.
  //cbi(Seg_LCD.digit[4],7);           //or clear
  sbi(Seg_LCD.digit[3],7);   //Set "Battery" and first tick.
  //cbi(Seg_LCD.digit[3],7);           // or clear
  */

  /*Example:
  //Count from zero to 99,999; show even numbers as positive, odds as negative:
  Seg_LCD.decimalPt=10; //Do not show decimal point.
  for (i=0; i<99999; i=i+1){
    //The fancy multiplier is to convert all odds to negative,
    //  all evens to positive:
    HT1621_Convert(i*(-1*(2*(i&1)-1)),&Seg_LCD);
    HT1621_Blanking(&Seg_LCD);  //Remove leading zeros.
    // Write the data prepared above, at once, set decimal point.
    HT1621_Refresh (&Seg_LCD); //4: No decimal point.
    AWU_Init(AWU_TIMEBASE_128MS); //Repeat after sleeping 128ms.
    halt();
  }
  */
  /* Example:  
//Here is some more demo code for displaying various numbers:
  while (1){ //Slowly scan through a few representative numbers:
    //Display 2:
    Seg_LCD.bat1=1;
    Seg_LCD.bat2=0;
    Seg_LCD.bat3=0;
    Seg_LCD.decimalPt=2;
    HT1621_Convert (-2,&Seg_LCD);
    HT1621_Blanking (&Seg_LCD);  //Remove leading zeros.
    HT1621_Refresh (&Seg_LCD); 
    AWU_Init(AWU_TIMEBASE_512MS);
    halt();

    //Display -249:
    Seg_LCD.bat1=1;
    Seg_LCD.bat2=1;
    Seg_LCD.bat3=0;
    Seg_LCD.decimalPt=1;
    HT1621_Convert (-249,&Seg_LCD);
    HT1621_Blanking (&Seg_LCD);  //Remove leading zeros.
    HT1621_Refresh (&Seg_LCD); 
    AWU_Init(AWU_TIMEBASE_512MS);
    halt();


    //Display 0:
    Seg_LCD.bat1=1;
    Seg_LCD.bat2=1;
    Seg_LCD.bat3=1;
    Seg_LCD.decimalPt=10;
    HT1621_Convert (0,&Seg_LCD);
    HT1621_Blanking (&Seg_LCD);  //Remove leading zeros.
    HT1621_Refresh (&Seg_LCD); 
    AWU_Init(AWU_TIMEBASE_512MS);
    halt();

    //Display 99999:
    Seg_LCD.bat1=0;
    Seg_LCD.bat2=0;
    Seg_LCD.bat3=0;
    Seg_LCD.decimalPt=4;
    HT1621_Convert (99999,&Seg_LCD);
    HT1621_Blanking (&Seg_LCD);  //Remove leading zeros.
    HT1621_Refresh (&Seg_LCD); 
    AWU_Init(AWU_TIMEBASE_512MS);
    halt();
    //Delay(500);
  }
  */
  
  
  /*Example: Manipulate individual digits. 
  HT1621_Write(0,map[6]);  //Individually write to digit 0
  HT1621_Write(2,map[5]);  //Individually write to digit 1
  HT1621_Write(4,map[4]);  //Individually write to digit 2
  HT1621_Write(6,map[3]);  //Individually write to digit 3
  HT1621_Write(8,map[2]);  //Individually write to digit 4
  HT1621_Write(10,map[1]); //Individually write to digit 5
  */

}


void AWU_ISR(void) __interrupt(AWU_INT)
//SDCC says that ISRs must reside within the main code file.
{
  AWU_GetFlagStatus();  //Clear AWU peripheral pending bit
}


//-------------------------------------------------------------------------
// Setup ADC1 to perform a single conversion and then generate an interrupt.
void SetupADC()
{
  GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_IN_FL_NO_IT); //ADC1, CH2: PC4
  
  ADC1_DeInit();
  
  ADC1_Init(ADC1_CONVERSIONMODE_SINGLE, ADC1_CHANNEL_2,     \
            ADC1_PRESSEL_FCPU_D2,                           \
            ADC1_EXTTRIG_TIM, DISABLE,                      \
            ADC1_ALIGN_RIGHT, ADC1_SCHMITTTRIG_CHANNEL2,    \
            DISABLE);
  
  ADC1_ITConfig(ADC1_IT_EOCIE, DISABLE);  // Disable EOC interrupt.
  // For the first argument, see stm8_adc1.h; ADC1_IT_TypeDef
}


