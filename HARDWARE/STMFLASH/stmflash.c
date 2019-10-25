#include "stmflash.h"
#include "delay.h"
#include "usart.h"
 
//////////////////////////////////////////////////////////////////////////////////	 

//��ȡָ����ַ�İ���(16λ����)
//faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
//����ֵ:��Ӧ����.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}

//������д��
//WriteAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��   
void STMFLASH_Write_NoCheck(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0; i<NumToWrite; i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//��ַ����2.
	}  
}

//��ָ����ַ��ʼд��ָ�����ȵ�����
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
//pBuffer:����ָ��
//NumToWrite:����(16λ)��(����Ҫд���16λ���ݵĸ���.)
u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//�����2K�ֽ�
void STMFLASH_Write(u32 WriteAddr, u16 *pBuffer, u16 NumToWrite)	
{
	u32 secpos;	   //������ַ
	u16 secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
	u16 secremain; //������ʣ���ַ(16λ�ּ���)	   
 	u16 i;    
	u32 offaddr;   //ȥ��0X08000000��ĵ�ַ
	
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))
		return;//�Ƿ���ַ
	
	FLASH_Unlock();						//����
	
	offaddr = WriteAddr-STM32_FLASH_BASE;	//ʵ��ƫ�Ƶ�ַ.
	secpos = offaddr/STM_SECTOR_SIZE;			//������ַ  0~127 for STM32F103RBT6
	secoff = (offaddr%STM_SECTOR_SIZE)/2;		//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
	secremain = STM_SECTOR_SIZE/2-secoff;		//����ʣ��ռ��С   
	if( NumToWrite <= secremain )
		secremain = NumToWrite;//�����ڸ�������Χ
	
	STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE/2);//������������������
	for(i=0; i<secremain; i++)//У������
	{
		if(STMFLASH_BUF[secoff+i] != 0XFFFF)
			break;//��Ҫ����  	  
	}
		
	if(i<secremain)//��Ҫ����
	{
		FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//�����������
		for(i=0;i<secremain;i++)//����
		{
			STMFLASH_BUF[i+secoff] = pBuffer[i];	  
		}
		STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE, STMFLASH_BUF, STM_SECTOR_SIZE/2);//д����������  
	}
	else
		STMFLASH_Write_NoCheck(WriteAddr, pBuffer, secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 		

	FLASH_Lock();//����
}

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��
void STMFLASH_Read(u32 ReadAddr, u16 *pBuffer, u16 NumToRead)   	
{
	u16 i;
	for(i=0; i<NumToRead; i++)
	{
		pBuffer[i] = STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
		ReadAddr += 2;//ƫ��2���ֽ�.	
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















