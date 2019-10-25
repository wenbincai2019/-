#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"



#define CHECK_LOW_TEMP  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_9)
#define KEY1  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)
#define KEY2  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)



void KEY_Init(void);//IO≥ı ºªØ

#endif
