#include "sys.h"
#include "usart.h"	  
#include "stmflash.h"
#include "led.h"
#include "string.h"
#include "delay.h"

////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif

#define	HEAD_BYTE1	0xF7
#define	HEAD_BYTE2	0x89


u8  USART_RX_BUF[USART_BUF_LEN];

u16 USART_RX_STA = 0;       //接收状态标记	 
u16 Receive_Len = 0;

float g_pwm_value_config[256] = {0};
float g_pwm_value_config2[256] = {0};

u16 g_pwm_frequency = 2;

u16 g_light_operation_delay = 0;
u16 g_day_night_operation_delay = 0;

u8 g_day_mode = 0; 		//0:light; 1:night
u8 g_light_scale_now = 0;
u8 g_light_scale_next = 0;

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

void Get_Pwm_Frequency_Config(void)
{
	g_pwm_frequency = Read_Light_Pwm();
	if( g_pwm_frequency < 1 || g_pwm_frequency > 10 )
		g_pwm_frequency = 2;
}

void Get_Pwm_value_Config(void)
{
	int i;
	u8 tempbuffer[256] = {0};

	for(i=0; i<256; i++)
	{
		g_pwm_value_config[i] = 0;
		g_pwm_value_config2[i] = 0;
	}
	
	Read_Light_Value(tempbuffer, ADDR_CONFIG_LIGHT_VALUE_0);
	for( i=0; i<64; i++ )
	{
		memcpy(&g_pwm_value_config[i], tempbuffer+i*4, 4);
		if( g_pwm_value_config[i] < 0.01 || g_pwm_value_config[i] > 100.00 )
		{
			g_pwm_value_config[i] = 0;
		}
	}

	Read_Light_Value(tempbuffer, ADDR_CONFIG_LIGHT_VALUE_64);
	for( i=0; i<64; i++ )
	{
		memcpy(&g_pwm_value_config[i+64], tempbuffer+i*4, 4);
		if( g_pwm_value_config[i+64] < 0.01 || g_pwm_value_config[i+64] > 100.00 )
		{
			g_pwm_value_config[i+64] = 0;
		}
	}
	
	Read_Light_Value(tempbuffer, ADDR_CONFIG_LIGHT_VALUE_128);
	for( i=0; i<64; i++ )
	{
		memcpy(&g_pwm_value_config[i+128], tempbuffer+i*4, 4);
		if( g_pwm_value_config[i+128] < 0.01 || g_pwm_value_config[i+128] > 100.00 )
		{
			g_pwm_value_config[i+128] = 0;
		}
	}
	
	Read_Light_Value(tempbuffer, ADDR_CONFIG_LIGHT_VALUE_192);
	for( i=0; i<64; i++ )
	{
		memcpy(&g_pwm_value_config[i+192], tempbuffer+i*4, 4);
		if( g_pwm_value_config[i+192] < 0.01 || g_pwm_value_config[i+192] > 100.00 )
		{
			g_pwm_value_config[i+192] = 0;
		}
	}
	
	
	Read_Light_Value(tempbuffer, ADDR_CONFIG_NIGHT_VALUE_0);
	for( i=0; i<64; i++ )
	{
		memcpy(&g_pwm_value_config2[i], tempbuffer+i*4, 4);
		if( g_pwm_value_config2[i] < 0.01 || g_pwm_value_config2[i] > 100.00 )
		{
			g_pwm_value_config2[i] = 0;
		}
	}

	Read_Light_Value(tempbuffer, ADDR_CONFIG_NIGHT_VALUE_64);
	for( i=0; i<64; i++ )
	{
		memcpy(&g_pwm_value_config2[i+64], tempbuffer+i*4, 4);
		if( g_pwm_value_config2[i+64] < 0.01 || g_pwm_value_config2[i+64] > 100.00 )
		{
			g_pwm_value_config2[i+64] = 0;
		}
	}
	
	Read_Light_Value(tempbuffer, ADDR_CONFIG_NIGHT_VALUE_128);
	for( i=0; i<64; i++ )
	{
		memcpy(&g_pwm_value_config2[i+128], tempbuffer+i*4, 4);
		if( g_pwm_value_config2[i+128] < 0.01 || g_pwm_value_config2[i+128] > 100.00 )
		{
			g_pwm_value_config2[i+128] = 0;
		}
	}
	
	Read_Light_Value(tempbuffer, ADDR_CONFIG_NIGHT_VALUE_192);
	for( i=0; i<64; i++ )
	{
		memcpy(&g_pwm_value_config2[i+192], tempbuffer+i*4, 4);
		if( g_pwm_value_config2[i+192] < 0.01 || g_pwm_value_config2[i+192] > 100.00 )
		{
			g_pwm_value_config2[i+192] = 0;
		}
	}
}

void uart1_init(u32 bound)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
  //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 
}

static int check_receive_data(u8* buffer, u16 buffer_len, u8 check_data)
{
	u8 value = 0;
	u16 loop;
	for(loop=0; loop<buffer_len; loop++)
	{
		value += buffer[loop];
	}
	
	if( value != check_data )
		return 1;
	else
		return 0;
}

void Deal_With_Command(u8* command, u16 length)
{
	u8 send_buffer[USART_BUF_LEN] = {0};

	switch(command[0])
	{
		case 0x31:
		{
			u8 i;
			send_buffer[0] = 0x31;
			for(i=1; i<10; i++)
			{
				send_buffer[i] = i;
			}
			send_buffer[10] = 0;
			Send_From_Uart1(send_buffer, 11);
			
			break;
		}
		
		case 0x20:
		{
			u16 temp_frequency;
			send_buffer[0] = 0x20;
			memcpy(&temp_frequency, command+1, 2);
			if( temp_frequency <= 0 || temp_frequency > 10 )
			{
				send_buffer[1] = 0xff;
			}
			else
			{
				Set_Light_Frequency(temp_frequency);
				send_buffer[1] = 0;
			}
			Send_From_Uart1(send_buffer, 2);
			
			break;
		}
		
		case 0x21:
		{
			u16 temp_frequency;
			send_buffer[0] = 0x21;
			memcpy(&temp_frequency, command+1, 2);
			if( temp_frequency <= 0 || temp_frequency > 10 )
			{
				send_buffer[1] = 0xff;
			}
			else
			{
				Set_Light_Frequency(temp_frequency);
				Write_Light_Pwm(temp_frequency);
				send_buffer[1] = 0;
			}
			Send_From_Uart1(send_buffer, 2);
			
			break;
		}
		
		case 0x22:
		{
			send_buffer[0] = 0x22;
			memcpy(send_buffer+1, &g_pwm_frequency, 2);
			Send_From_Uart1(send_buffer, 3);
			break;
		}
		
		case 0x23:
		{
			float temp_pwm;
			send_buffer[0] = 0x23;
			memcpy(&temp_pwm, command+1, 4);
			if( temp_pwm < 0 || temp_pwm > 100 )
			{
				send_buffer[1] = 0xff;
			}
			else
			{
				Set_Light_Pwm(temp_pwm);
				send_buffer[1] = 0;
			}
			Send_From_Uart1(send_buffer, 2);
			break;
		}
		
		case 0x24:
		{
			u16 temp_index;
			send_buffer[0] = 0x24;
			memcpy(&temp_index, command+1, 2);
			if( temp_index > 255 )
			{
				send_buffer[1] = 0xff;
			}
			else
			{
				if( g_day_mode == 0 )
					Set_Light_Pwm(g_pwm_value_config[temp_index]);
				else
					Set_Light_Pwm(g_pwm_value_config2[temp_index]);
				
				send_buffer[1] = 0;
			}
			Send_From_Uart1(send_buffer, 2);
			break;
		}
		
		case 0x25:
		{
			send_buffer[0] = 0x25;
			if( command[1] != 0 && command[1] != 1 )
			{
				send_buffer[1] = 0xff;
			}
			else
			{
				g_day_mode = command[1];
				send_buffer[1] = 0;
			}

			Send_From_Uart1(send_buffer, 2);
			break;
		}
		
		case 0x26:
		{
			send_buffer[0] = 0x26;
			send_buffer[1] = g_day_mode;
			Send_From_Uart1(send_buffer, 2);
			break;
		}
		
		case 0x40:
		{
			int i;
			send_buffer[0] = 0x40;
			
			for( i=0; i<64; i++ )
			{
				memcpy(&g_pwm_value_config[i], command+(i*4+1), 4);
				if( g_pwm_value_config[i] < 0.00 || g_pwm_value_config[i] > 100.00 )
				{
					g_pwm_value_config[i] = 0;
					send_buffer[1] = 0xff;
					Send_From_Uart1(send_buffer, 2);
					return;
				}
			}
			
			Write_Light_Value(command+1, ADDR_CONFIG_LIGHT_VALUE_0);
			send_buffer[1] = 0;
			Send_From_Uart1(send_buffer, 2);
			break;
		}
		
		case 0x41:
		{
			int i;
			send_buffer[0] = 0x41;
			
			for( i=0; i<64; i++ )
			{
				memcpy(&g_pwm_value_config[i+64], command+(i*4+1), 4);
				if( g_pwm_value_config[i+64] < 0.00 || g_pwm_value_config[i+64] > 100.00 )
				{
					g_pwm_value_config[i+64] = 0;
					send_buffer[1] = 0xff;
					Send_From_Uart1(send_buffer, 2);
					return;
				}
			}
			
			Write_Light_Value(command+1, ADDR_CONFIG_LIGHT_VALUE_64);
			send_buffer[1] = 0;
			Send_From_Uart1(send_buffer, 2);
			break;
		}
		
		case 0x42:
		{
			int i;
			send_buffer[0] = 0x42;
			
			for( i=0; i<64; i++ )
			{
				memcpy(&g_pwm_value_config[i+128], command+(i*4+1), 4);
				if( g_pwm_value_config[i+128] < 0.00 || g_pwm_value_config[i+128] > 100.00 )
				{
					g_pwm_value_config[i+128] = 0;
					send_buffer[1] = 0xff;
					Send_From_Uart1(send_buffer, 2);
					return;
				}
			}
			
			Write_Light_Value(command+1, ADDR_CONFIG_LIGHT_VALUE_128);
			send_buffer[1] = 0;
			Send_From_Uart1(send_buffer, 2);
			break;
		}
		
		case 0x43:
		{
			int i;
			send_buffer[0] = 0x43;
			
			for( i=0; i<64; i++ )
			{
				memcpy(&g_pwm_value_config[i+192], command+(i*4+1), 4);
				if( g_pwm_value_config[i+192] < 0.00 || g_pwm_value_config[i+192] > 100.00 )
				{
					g_pwm_value_config[i+192] = 0;
					send_buffer[1] = 0xff;
					Send_From_Uart1(send_buffer, 2);
					return;
				}
			}
			
			Write_Light_Value(command+1, ADDR_CONFIG_LIGHT_VALUE_192);
			send_buffer[1] = 0;
			Send_From_Uart1(send_buffer, 2);
			break;
		}
		
		case 0x44:
		{
			int i;
			send_buffer[0] = 0x44;
			
			for( i=0; i<64; i++ )
			{
				memcpy(&g_pwm_value_config2[i], command+(i*4+1), 4);
				if( g_pwm_value_config2[i] < 0.00 || g_pwm_value_config2[i] > 100.00 )
				{
					g_pwm_value_config2[i] = 0;
					send_buffer[1] = 0xff;
					Send_From_Uart1(send_buffer, 2);
					return;
				}
			}
			
			Write_Light_Value(command+1, ADDR_CONFIG_NIGHT_VALUE_0);
			send_buffer[1] = 0;
			Send_From_Uart1(send_buffer, 2);
			break;
		}
		
		case 0x45:
		{
			int i;
			send_buffer[0] = 0x45;
			
			for( i=0; i<64; i++ )
			{
				memcpy(&g_pwm_value_config2[i+64], command+(i*4+1), 4);
				if( g_pwm_value_config2[i+64] < 0.00 || g_pwm_value_config2[i+64] > 100.00 )
				{
					g_pwm_value_config2[i+64] = 0;
					send_buffer[1] = 0xff;
					Send_From_Uart1(send_buffer, 2);
					return;
				}
			}
			
			Write_Light_Value(command+1, ADDR_CONFIG_NIGHT_VALUE_64);
			send_buffer[1] = 0;
			Send_From_Uart1(send_buffer, 2);
			break;
		}
		
		case 0x46:
		{
			int i;
			send_buffer[0] = 0x46;
			
			for( i=0; i<64; i++ )
			{
				memcpy(&g_pwm_value_config2[i+128], command+(i*4+1), 4);
				if( g_pwm_value_config2[i+128] < 0.00 || g_pwm_value_config2[i+128] > 100.00 )
				{
					g_pwm_value_config2[i+128] = 0;
					send_buffer[1] = 0xff;
					Send_From_Uart1(send_buffer, 2);
					return;
				}
			}
			
			Write_Light_Value(command+1, ADDR_CONFIG_NIGHT_VALUE_128);
			send_buffer[1] = 0;
			Send_From_Uart1(send_buffer, 2);
			break;
		}
		
		case 0x47:
		{
			int i;
			send_buffer[0] = 0x47;
			
			for( i=0; i<64; i++ )
			{
				memcpy(&g_pwm_value_config2[i+192], command+(i*4+1), 4);
				if( g_pwm_value_config2[i+192] < 0.00 || g_pwm_value_config2[i+192] > 100.00 )
				{
					g_pwm_value_config2[i+192] = 0;
					send_buffer[1] = 0xff;
					Send_From_Uart1(send_buffer, 2);
					return;
				}
			}
			
			Write_Light_Value(command+1, ADDR_CONFIG_NIGHT_VALUE_192);
			send_buffer[1] = 0;
			Send_From_Uart1(send_buffer, 2);
			break;
		}
		
		case 0x50:
		{
			send_buffer[0] = 0x50;
			Read_Light_Value(send_buffer+1, ADDR_CONFIG_LIGHT_VALUE_0);
			Send_From_Uart1(send_buffer, 257);
			break;
		}
		
		case 0x51:
		{
			send_buffer[0] = 0x51;
			Read_Light_Value(send_buffer+1, ADDR_CONFIG_LIGHT_VALUE_64);
			Send_From_Uart1(send_buffer, 257);
			break;
		}
		
		case 0x52:
		{
			send_buffer[0] = 0x52;
			Read_Light_Value(send_buffer+1, ADDR_CONFIG_LIGHT_VALUE_128);
			Send_From_Uart1(send_buffer, 257);
			break;
		}
		
		case 0x53:
		{
			send_buffer[0] = 0x53;
			Read_Light_Value(send_buffer+1, ADDR_CONFIG_LIGHT_VALUE_192);
			Send_From_Uart1(send_buffer, 257);
			break;
		}
		
		case 0x54:
		{
			send_buffer[0] = 0x54;
			Read_Light_Value(send_buffer+1, ADDR_CONFIG_NIGHT_VALUE_0);
			Send_From_Uart1(send_buffer, 257);
			break;
		}
		
		case 0x55:
		{
			send_buffer[0] = 0x55;
			Read_Light_Value(send_buffer+1, ADDR_CONFIG_NIGHT_VALUE_64);
			Send_From_Uart1(send_buffer, 257);
			break;
		}
		
		case 0x56:
		{
			send_buffer[0] = 0x56;
			Read_Light_Value(send_buffer+1, ADDR_CONFIG_NIGHT_VALUE_128);
			Send_From_Uart1(send_buffer, 257);
			break;
		}
		
		case 0x57:
		{
			send_buffer[0] = 0x57;
			Read_Light_Value(send_buffer+1, ADDR_CONFIG_NIGHT_VALUE_192);
			Send_From_Uart1(send_buffer, 257);
			break;
		}
		
		default:
			break;
	}
}

u8 recv_length_tag = 0;
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
	u8 Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
	{
			Res = USART_ReceiveData(USART1);	//读取接收到的数据
		
			if(USART_RX_STA&0x4000)		//start
			{
				if( (recv_length_tag&0x01) == 0 )
				{
					Receive_Len = (u16)Res<<8;
					recv_length_tag |= 0x01;
				}
				else if( (recv_length_tag&0x02) == 0 )
				{
					Receive_Len |= Res;
					recv_length_tag |= 0x02;
				}
				else
				{
					if( (USART_RX_STA&0xfff) == Receive_Len )
					{
						if(!check_receive_data(USART_RX_BUF, (USART_RX_STA&0xfff), Res))
						{
							Deal_With_Command(USART_RX_BUF, USART_RX_STA&0xfff);
						}

						USART_RX_STA = 0;
						recv_length_tag = 0;
					}
					else
					{
						USART_RX_BUF[USART_RX_STA&0xfff] = Res ;
						USART_RX_STA++;
					}
				}
			}
			else
			{
				if(USART_RX_STA&0x2000)
				{
					if( Res != HEAD_BYTE2 )
					{
						USART_RX_STA = 0;
					}
					else
					{
						USART_RX_STA |= 0x4000;
						Receive_Len = 0;
					}
				}
				else
				{
					if( Res == HEAD_BYTE1 )
						USART_RX_STA |= 0x2000;
				}	 
			}
		}
}

void Send_From_Uart1(u8* buffer, u16 length)
{
	u8 check_value = 0;
	u16 loop;
	
	if( buffer == NULL || length == 0 || length > USART_BUF_LEN-4 )
		return;
	
	USART_SendData(USART1, HEAD_BYTE1);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	USART_SendData(USART1, HEAD_BYTE2);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	
	USART_SendData(USART1, (length>>8));
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	USART_SendData(USART1, (length&0xff));
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	
	for(loop=0; loop<length; loop++)
	{
		USART_SendData(USART1, buffer[loop]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
		
		check_value += buffer[loop];
	}
	
	USART_SendData(USART1, check_value);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
}


void Set_Light_Frequency(u16 value)
{
	u16 psc_value = 0;
	
	if( value <= 0 || value > 10 )
		return;
	
	psc_value = 20/value;
	TIM_PrescalerConfig(TIM4, psc_value-1, TIM_PSCReloadMode_Immediate);
	g_pwm_frequency = value;
}

void Set_Light_Pwm(float fvalue)
{
	u16 value;
	
	if( fvalue > 100.00 || fvalue < 0.0)
		return;
	
	value = (u16)(fvalue*360);
	TIM_SetCompare3(TIM4, value);	
}

void Set_Light_Scale(u8 scale)
{
	if( g_day_mode == 0 )
		Set_Light_Pwm(g_pwm_value_config[scale]);
	else
		Set_Light_Pwm(g_pwm_value_config2[scale]);
}

void uart2_init(u32 bound)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  
	//USART2_TX
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  //USART2_RX
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //Usart2 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2 ;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
  //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART2, &USART_InitStructure); //初始化串口
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART2, ENABLE);                    //使能串口
}


void USART2_IRQHandler(void)   
{
	u8 Res;

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		Res = USART_ReceiveData(USART2);
		if( g_light_operation_delay > 0 )
		{
			g_light_scale_next = Res;
			g_light_operation_delay = 0;
			g_day_night_operation_delay = 0;
			
			USART_SendData(USART2, Res);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
		}
		else if( g_day_night_operation_delay > 0 )
		{
			if( Res == 0 )
			{
				g_day_mode = 1;
				Set_Light_Pwm(g_pwm_value_config2[g_light_scale_next]);
			}
			else if ( Res == 1 )
			{
				g_day_mode = 0;
				Set_Light_Pwm(g_pwm_value_config[g_light_scale_next]);
			}
			
			g_light_scale_now = g_light_scale_next;
			g_light_operation_delay = 0;
			g_day_night_operation_delay = 0;
			
			USART_SendData(USART2, Res);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
		}
		else if( Res == 0x1f )		//Handshake
		{
			USART_SendData(USART2, Res);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
		}
		else if( Res == 0x00 )
		{
			USART_SendData(USART2, Res);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
			g_light_operation_delay = 200;
		}
		else if( Res == 0x0a )
		{
			USART_SendData(USART2, Res);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
			g_day_night_operation_delay = 200;
		}
		else if( Res == 0x30 )
		{
			USART_SendData(USART2, Res);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
			USART_SendData(USART2, 0x00);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
		}
		else if( Res == 0x36 )
		{
			USART_SendData(USART2, Res);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
			USART_SendData(USART2, 0xfb);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
		}
		else if( Res == 0x35 )
		{
			USART_SendData(USART2, Res);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
			USART_SendData(USART2, 0x37);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
		}
	}
}

void Set_Init_Pwm(void)
{
	float init_pwm = 0.0;
	
	if( g_pwm_value_config[255] < 0.01 )
	{
		Set_Light_Pwm(70);
	}
	else
	{
		init_pwm = g_pwm_value_config[255]*0.7;
		Set_Light_Pwm(init_pwm);
	}
}

