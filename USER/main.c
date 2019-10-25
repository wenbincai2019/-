#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "timer.h"
#include "string.h"

extern u16 g_pwm_frequency;

extern u8 g_light_scale_now;
extern u8 g_light_scale_next;

int main(void)
{
	int loop = 1;
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	
	Get_Pwm_Frequency_Config();
	Get_Pwm_value_Config();
	
	uart1_init(115200);	 //串口初始化为115200
	uart2_init(9600);
	
	TIM4_PWM_Init(35999, 19);	//PWM频率=72000000/36000/20=100hz
	
	Set_Light_Frequency(g_pwm_frequency);
	
	delay_ms(1000);
	delay_ms(600);
	Set_Init_Pwm();
	
	TIM1_Refresh_Init(9, 7199);		//1ms update one time

  while(1)
	{
		if( (loop%20) == 0 )
		{
			if( g_light_scale_now < g_light_scale_next )
			{
				g_light_scale_now++;
				Set_Light_Scale(g_light_scale_now);
			}
			else if ( g_light_scale_now > g_light_scale_next )
			{
				g_light_scale_now--;
				Set_Light_Scale(g_light_scale_now);
			}
		}
		
		loop++;
	
		delay_ms(1);
	}	 
}

