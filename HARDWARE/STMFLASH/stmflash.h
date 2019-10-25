#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"  


//////////////////////////////////////////////////////////////////////////////////////////////////////
//用户根据自己的需要设置
#define STM32_FLASH_SIZE 128 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM_SECTOR_SIZE	1024
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH起始地址
#define STM32_FLASH_BASE 				0x08000000 	//STM32 FLASH的起始地址
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

void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//从指定地址开始读出指定长度的数据

void Write_Light_Pwm(u16 value);
u16	 Read_Light_Pwm(void);

void Write_Light_Value(u8* pBuffer, u16 addr);
u16 Read_Light_Value(u8* pBuffer, u16 addr);

#endif

















