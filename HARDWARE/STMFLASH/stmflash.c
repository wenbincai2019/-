#include "stmflash.h"
#include "delay.h"
#include "usart.h"
 
//////////////////////////////////////////////////////////////////////////////////	 

//读取指定地址的半字(16位数据)
//faddr:读地址(此地址必须为2的倍数!!)
//返回值:对应数据.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}

//不检查的写入
//WriteAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数   
void STMFLASH_Write_NoCheck(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0; i<NumToWrite; i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//地址增加2.
	}  
}

//从指定地址开始写入指定长度的数据
//WriteAddr:起始地址(此地址必须为2的倍数!!)
//pBuffer:数据指针
//NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//最多是2K字节
void STMFLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)	
{
	u32 secpos;	   //扇区地址
	u16 secoff;	   //扇区内偏移地址(16位字计算)
	u16 secremain; //扇区内剩余地址(16位字计算)	   
 	u16 i;    
	u32 offaddr;   //去掉0X08000000后的地址
	
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))
		return;//非法地址
	
	FLASH_Unlock();						//解锁
	
	offaddr = WriteAddr-STM32_FLASH_BASE;	//实际偏移地址.
	secpos = offaddr/STM_SECTOR_SIZE;			//扇区地址  0~127 for STM32F103RBT6
	secoff = (offaddr%STM_SECTOR_SIZE)/2;		//在扇区内的偏移(2个字节为基本单位.)
	secremain = STM_SECTOR_SIZE/2-secoff;		//扇区剩余空间大小   
	if( NumToWrite <= secremain )
		secremain = NumToWrite;//不大于该扇区范围
	
	STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE/2);//读出整个扇区的内容
	for(i=0; i<secremain; i++)//校验数据
	{
		if(STMFLASH_BUF[secoff+i] != 0XFFFF)
			break;//需要擦除  	  
	}
		
	if(i<secremain)//需要擦除
	{
		FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//擦除这个扇区
		for(i=0;i<secremain;i++)//复制
		{
			STMFLASH_BUF[i+secoff] = pBuffer[i];	  
		}
		STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE/2);//写入整个扇区  
	}
	else
		STMFLASH_Write_NoCheck(WriteAddr, pBuffer, secremain);//写已经擦除了的,直接写入扇区剩余区间. 		

	FLASH_Lock();//上锁
}

//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToWrite:半字(16位)数
void STMFLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead)   	
{
	u16 i;
	for(i=0; i<NumToRead; i++)
	{
		pBuffer[i] = STMFLASH_ReadHalfWord(ReadAddr);//读取2个字节.
		ReadAddr += 2;//偏移2个字节.	
	}
}

void Write_Light_Pwm(u16 value)
{
	STMFLASH_Write(STM32_CONFIG_ADDR_BASE+ADDR_CONFIG_LIGHT_PWM, &value, 1);
}

u16	Read_Light_Pwm(void)
{
	u16 value = 0;
	
	STMFLASH_Read(STM32_CONFIG_ADDR_BASE+ADDR_CONFIG_LIGHT_PWM, &value, 1);
	
	return value;
}

void Write_Light_Value(u8* pBuffer, u16 addr)
{

	STMFLASH_Write(STM32_CONFIG_ADDR_BASE+addr, (u16*)pBuffer, 0x100/2);
}

u16 Read_Light_Value(u8* pBuffer, u16 addr)
{
	STMFLASH_Read(STM32_CONFIG_ADDR_BASE+addr, (u16*)pBuffer, 0x100/2);
	
	return 0;
}















