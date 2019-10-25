#include "adc.h"
#include "delay.h"

#define		CAPADC		(ADC1)

void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |RCC_APB2Periph_ADC1, ENABLE );	  //使能ADC1通道时钟
 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
                      
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	ADC_DeInit(CAPADC);  //复位ADC1 

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(CAPADC, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   


	ADC_Cmd(CAPADC, ENABLE);	//使能指定的ADC1
	ADC_ResetCalibration(CAPADC);	//使能复位校准  
	while(ADC_GetResetCalibrationStatus(CAPADC));	//等待复位校准结束
	ADC_StartCalibration(CAPADC);	 //开启AD校准
	while(ADC_GetCalibrationStatus(CAPADC));	 //等待校准结束
}


//获得ADC值
u16 Get_Adc(u8 ch, u8 rank)   
{
  u8 timeout = 0;
	
	ADC_RegularChannelConfig(CAPADC, ch, rank, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(CAPADC, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(CAPADC, ADC_FLAG_EOC ))	//等待转换结束
	{
		timeout++;
		if( timeout > 10 )
			return 0;
	}

	return ADC_GetConversionValue(CAPADC);	//返回最近一次ADC1规则组的转换结果
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

































