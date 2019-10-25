#include "adc.h"
#include "delay.h"

#define		CAPADC		(ADC1)

void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
                      
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	ADC_DeInit(CAPADC);  //��λADC1 

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(CAPADC, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   


	ADC_Cmd(CAPADC, ENABLE);	//ʹ��ָ����ADC1
	ADC_ResetCalibration(CAPADC);	//ʹ�ܸ�λУ׼  
	while(ADC_GetResetCalibrationStatus(CAPADC));	//�ȴ���λУ׼����
	ADC_StartCalibration(CAPADC);	 //����ADУ׼
	while(ADC_GetCalibrationStatus(CAPADC));	 //�ȴ�У׼����
}


//���ADCֵ
u16 Get_Adc(u8 ch, u8 rank)   
{
  u8 timeout = 0;
	
	ADC_RegularChannelConfig(CAPADC, ch, rank, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(CAPADC, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(CAPADC, ADC_FLAG_EOC ))	//�ȴ�ת������
	{
		timeout++;
		if( timeout > 10 )
			return 0;
	}

	return ADC_GetConversionValue(CAPADC);	//�������һ��ADC1�������ת�����
}

u32 Get_Adc_Average(u8 ch, u8 rank, u8 times)
{
	u32 temp_val=0;
	u8 t;
	
	Get_Adc(ch, rank);
	delay_ms(5);
	
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch, rank);
		delay_ms(5);
	}
	return temp_val/times;
}

int Get_Tempture(void)
{
	u32 adc_value;
	int temp;
	float ftemp, fvoltage;
	
	adc_value = Get_Adc_Average(10, 1, 1);
	fvoltage = (float)adc_value*(3.3/4096);
	ftemp = (fvoltage/4020)*1000000-273.2;
	temp = ftemp;
	
	return temp;
}

































