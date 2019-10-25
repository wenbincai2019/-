#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"  


//////////////////////////////////////////////////////////////////////////////////////////////////////
//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 128 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM_SECTOR_SIZE	1024
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 				0x08000000 	//STM32 FLASH����ʼ��ַ
#define	STM32_CONFIG_ADDR_BASE	0x0801F000



#define	ADDR_CONFIG_LIGHT_PWM					0x00
#define	ADDR_CONFIG_LIGHT_VALUE_0			0x100
#define	ADDR_CONFIG_LIGHT_VALUE_64		0x200
#define	ADDR_CONFIG_LIGHT_VALUE_128		0x300
#define	ADDR_CONFIG_LIGHT_VALUE_192		0x400
#define	ADDR_CONFIG_NIGHT_VALUE_0			0x500
#define	ADDR_CONFIG_NIGHT_VALUE_64		0x600
#define	ADDR_CONFIG_NIGHT_VALUE_128		0x700
#define	ADDR_CONFIG_NIGHT_VALUE_192		0x800

void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����

void Write_Light_Pwm(u16 value);
u16	 Read_Light_Pwm(void);

void Write_Light_Value(u8* pBuffer, u16 addr);
u16 Read_Light_Value(u8* pBuffer, u16 addr);

#endif

















