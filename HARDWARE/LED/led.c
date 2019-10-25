#include "led.h"



void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
 
void Set_Led(FunctionalState NewState)
{
	if( NewState == ENABLE )
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_8);
	}
	else
	{
		GPIO_SetBits(GPIOC, GPIO_Pin_8);
	}
}

