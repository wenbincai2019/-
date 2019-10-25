#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

//All rights reserved
//********************************************************************************

#define USART_BUF_LEN  			(256+8)  	//定义最大接收字节数 



void uart1_init(u32 bound);
void Send_From_Uart1(u8* buffer, u16 length);

void Get_Pwm_Frequency_Config(void);
void Get_Pwm_value_Config(void);

void Set_Light_Frequency(u16 value);
void Set_Light_Pwm(float fvalue);
void Set_Light_Scale(u8 scale);

void uart2_init(u32 bound);

void Set_Init_Pwm(void);

#endif


