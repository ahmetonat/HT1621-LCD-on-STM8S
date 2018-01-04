
#include "HT1621.h"
#include "awu.h"

void HT1621_SendBits(uint8_t sdata,uint8_t cnt) //Send cnt MSB bits of sdata.
{ 
  uint8_t i; 
  for(i=0;i<cnt;i++) 
    { 
      WR0;
      if(sdata&0x80){
	DATA1;
      }else{
	DATA0;
      }
      WR1;
      sdata<<=1; 
    } 
}

void HT1621_SendCmd(uint8_t command) //Write to the command registers.
{ 
  CS0;                     //Select,
  HT1621_SendBits(0x80,4); //Next byte is a command, (implemented as 1000+8
                           // bits, rather than the 100+9 bits specified
                           // in the datasheet.)
  HT1621_SendBits(command,8);//Write the 8 bit command value,
  CS1;                     //Deselect
}

void HT1621_Write(uint8_t addr,uint8_t sdata) //Write sdata to address addr:
{ 
  addr<<=2; 
  CS0; 
  HT1621_SendBits(0xa0,3);     //Write following to data memory
  HT1621_SendBits(addr,6);     // at address addr
  HT1621_SendBits(sdata,8);    // this is the data to be written.
  CS1; 
} 

void HT1621_AllOff(uint8_t number) //Clear all segments.
{
  uint8_t i; 
  uint8_t addr=0; 
  for(i=0;i<number;i++) 
    { 
      HT1621_Write(addr,0x00); 
      addr+=2; 
    } 
}

void HT1621_AllOn(uint8_t number) //Set all segments.
{
  uint8_t i; 
  uint8_t addr=0; 
  for(i=0;i<number;i++) 
    {
      HT1621_Write(addr,0xff); 
      addr+=2; 
    } 
}

void HT1621_Init(void)//Initialize the HT1621 chip.
{
  CS1;
  DATA1;
  WR1;
  AWU_Init(AWU_TIMEBASE_512MS);
  halt();
  //  Delay(500);   //TODO: This can be shorter.
  HT1621_SendCmd(Sys_en);
  HT1621_SendCmd(RCosc);    
  HT1621_SendCmd(ComMode);  
  HT1621_SendCmd(LCD_on);
}    

void HT1621_Refresh(HT1621Values * LCD_data) 
  //Display the prepared number.
  //Set the decimal point at location p.
{
  uint8_t i=0;
  //Show the decimal point:
  if (LCD_data->decimalPt>0 &&LCD_data->decimalPt<=3){
      sbi(LCD_data->digit[LCD_data->decimalPt-1],7);
  }
  //Adjust the battery indicator:
  if (LCD_data->bat1)
    LCD_data->digit[3]|=0x80;
  if (LCD_data->bat2)
    LCD_data->digit[4]|=0x80;
  if (LCD_data->bat3)
    LCD_data->digit[5]|=0x80;

  //Show the number itself:
  for(i=0;i<=5;i++) 
    {
      HT1621_Write(i*2,LCD_data->digit[i]);
    }
}

void  HT1621_ValuesConstructor(HT1621Values * LCD_data){
/*0,1,2,3,4,5,6,7,8,9,A,b,C,c,d,E,F,H,h,L,n,N,o,P,r,t,U,-, ,*/
//{0x7D,0x60,0x3E,0x7A,0x63,0x5B,0x5F,0x70,0x7F,0x7B,0x77,0x4F,0x1D,0x0E,0x6E,0x1F,0x17,0x67,0x47,0x0D,0x46,0x75,0x37,0x06,0x0F,0x6D,0x02,0x00,};
//AO!: The figures and numbers do not match! 29 figures, 28 numbers.
  LCD_data->map[0]= 0x7D;  //'0'
  LCD_data->map[1]= 0x60;  //'1'
  LCD_data->map[2]= 0x3E;  //'2'
  LCD_data->map[3]= 0x7A;  //'3'
  LCD_data->map[4]= 0x63;  //'4'
  LCD_data->map[5]= 0x5B;  //'5'
  LCD_data->map[6]= 0x5F;  //'6'
  LCD_data->map[7]= 0x70;  //'7'
  LCD_data->map[8]= 0x7F;  //'8'
  LCD_data->map[9]= 0x7B;  //'9'
  LCD_data->map[10]= 0x77;  //'A'
  LCD_data->map[11]= 0x4F;  //'b'
  LCD_data->map[12]= 0x1D;  //'C'
  LCD_data->map[13]= 0x0E;  //'c'
  LCD_data->map[14]= 0x6E;  //'d'
  LCD_data->map[15]= 0x1F;  //'E'
  LCD_data->map[16]= 0x17;  //'F'
  LCD_data->map[17]= 0x67;  //'H'
  LCD_data->map[18]= 0x47;  //'h'
  LCD_data->map[19]= 0x0D;  //'L'
  LCD_data->map[20]= 0x46;  //'n'
  LCD_data->map[21]= 0x75;  //'N'
  LCD_data->map[22]= 0x37;  //'o'
  LCD_data->map[23]= 0x06;  //'P'
  LCD_data->map[24]= 0x0F;  //'r'
  LCD_data->map[25]= 0x6D;  //'T'
  LCD_data->map[26]= 0x02;  //'-'
  LCD_data->map[27]= 0x00;  //' '

  LCD_data->digit[0]=0x00;
  LCD_data->digit[1]=0x00;
  LCD_data->digit[2]=0x00;
  LCD_data->digit[3]=0x00;
  LCD_data->digit[4]=0x00;
  LCD_data->digit[5]=0x00;

  LCD_data->decimalPt=10; //Do not display decimal pt.

  LCD_data->bat1 =0; //Do not display battery sign.
  LCD_data->bat2 =0;
  LCD_data->bat3 =0;
}


void HT1621_Convert(long number, HT1621Values * LCD_data)
  {
  int CurrentDigit;
  int i;

  //Set divisor array: 
  const long divisors[]={1, 10, 100, 1000, 10000, 100000};
  const long MaxRepresentable = 99999;

    //Check that the number is smaller than MaxRepresentable
    if(number > MaxRepresentable){
      //printf("Number too big.\n");
      //Set all digits to '-'
      for(CurrentDigit=5;CurrentDigit>=0;--CurrentDigit){
	LCD_data->digit[CurrentDigit]=LCD_data->map[26]; //display '-'
      }
      return;
    }
    if (number <0){  //Check the sign of the number.
      number= -number;
      LCD_data->digit[5]=LCD_data->map[26]; //display '-'
    }else{
      LCD_data->digit[5]=LCD_data->map[27]; //display ' '
    }
    //Start from the highest digit, keep subtracting to obtain the value of
    // that digit. Move on to the next smaller digit.
    for(CurrentDigit = 4; CurrentDigit>=0; --CurrentDigit){
      LCD_data->digit[CurrentDigit]=0;
      i=0;
      while(number >= divisors[CurrentDigit]){
	  number=number-divisors[CurrentDigit];
	  ++i;
      }
      LCD_data->digit[CurrentDigit]=LCD_data->map[i];
    }
}


//Do not show the leading '0' in the number.
//If the number is negative, adjust it so that the negative sign
// appears to the left of the leftmost displayed digit.
//If there is a decimal point displayed, adjust blanking such that
// there is at least one zero to the left of decimal point
// and no blanking to the right of the decimal point.
void HT1621_Blanking (HT1621Values * LCD_data){
  uint8_t i;
  uint8_t s;
  uint8_t decimalPtSave;
  s= LCD_data->digit[5]; //Check the first digit. Copy it into variable s.

  decimalPtSave=0; //if decimal point was set to location 4, 
                   // it is an unusual case. Treated below:
  if (LCD_data->decimalPt==4){
    decimalPtSave=LCD_data->decimalPt;
    LCD_data->decimalPt=10;
  }

  for (i=4; i>0;--i){  //For i= digits '4' to '1'
    //If there is a decimal point here, do not blank further:
    if (LCD_data->decimalPt==i){
      LCD_data->digit[5]=LCD_data->map[27]; //Clear leftmost digit.
      LCD_data->digit[i+1]=s;//ifnonzero, copy s into the higher order digit.
      return;
    }//End: decimal point adjustment.
    if (LCD_data->digit[i]==LCD_data->map[0]){ //If digit itself is zero
	LCD_data->digit[i]=LCD_data->map[27];    //make it a blank.
    }else{
      LCD_data->digit[5]=LCD_data->map[27]; //Clear leftmost digit.
      LCD_data->digit[i+1]=s;//ifnonzero, copy s into the higher order digit.
      return;                //Return.
    }
  }
  //If we came all the way to here, we have reached the rightmost digit.
  //Bring the sign next to it:
  LCD_data->digit[5]=LCD_data->map[27]; //Clear leftmost digit.
  LCD_data->digit[1]=s;//ifnonzero, copy s into the higher order digit.
  if (decimalPtSave){
    LCD_data->decimalPt=4;
  }
}

void HT1621_PortInit(void){
  //Set GPIO for HT1621:
  GPIO_Init(HT1621Port, HT1621WR, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(HT1621Port, HT1621Data, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(HT1621Port, HT1621CS, GPIO_MODE_OUT_PP_LOW_FAST);

}


