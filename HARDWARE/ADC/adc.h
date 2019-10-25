#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"

//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

void  Adc_Init(void);

u16  Get_Adc(u8 ch, u8 rank); 
u32 Get_Adc_Average(u8 ch,u8 rank,u8 times); 
int Get_Tempture(void);
 
#endif 
