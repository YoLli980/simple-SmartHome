#include "adc.h"
#include "delay.h"
void Adc_Init(void)
{
		ADC_InitTypeDef ADC_InitStruce;
		GPIO_InitTypeDef GPIO_InitStruce;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA,ENABLE);
		RCC_ADCCLKConfig(RCC_PCLK2_Div6);
		GPIO_InitStruce.GPIO_Mode=GPIO_Mode_AIN;
		GPIO_InitStruce.GPIO_Pin=GPIO_Pin_1;
		GPIO_InitStruce.GPIO_Speed =GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStruce);
	
	
		
		ADC_DeInit(ADC1);
		ADC_InitStruce.ADC_ContinuousConvMode = DISABLE;
		ADC_InitStruce.ADC_DataAlign=ADC_DataAlign_Right;
		ADC_InitStruce.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
		
		ADC_InitStruce.ADC_Mode=ADC_Mode_Independent;
		ADC_InitStruce.ADC_NbrOfChannel = 1;
		ADC_InitStruce.ADC_ScanConvMode = DISABLE;
		ADC_Init(ADC1,&ADC_InitStruce);
		ADC_Cmd(ADC1,ENABLE);
	  
		ADC_ResetCalibration(ADC1);
		while(ADC_GetResetCalibrationStatus(ADC1));
		ADC_StartCalibration(ADC1);
	  while(ADC_GetCalibrationStatus(ADC1));
	
}


u16  Get_Adc(u8 ch)
{
		ADC_RegularChannelConfig(ADC1,ch,1,ADC_SampleTime_239Cycles5);
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);
		while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
		return ADC_GetConversionValue(ADC1);
}
u16 Get_Adc_Average(u8 ch,u8 times)
{
		u32 temp_val=0;
		u8 t;
		for(t=0;t<times;t++)
		{	
				temp_val+=Get_Adc(ch);
				delay_ms(5);
		}
		return temp_val/times;
}


//初始化ADC3
//这里我们仅以规则通道为例
//我们默认仅开启通道6																	   
void  Adc3_Init(void)
{      
	ADC_InitTypeDef ADC_InitStructure; 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3	, ENABLE );	  //使能ADC3通道时钟
	
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3,ENABLE);//ADC复位
	
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3,DISABLE);//复位结束	    
	
	ADC_DeInit(ADC3);  //复位ADC3,将外设 ADC3的全部寄存器重设为缺省值
	
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式: 独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC3, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器  
	
	
	ADC_Cmd(ADC3, ENABLE);	//使能指定的ADC3
	
	ADC_ResetCalibration(ADC3);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC3));	//等待复位校准结束
	
	ADC_StartCalibration(ADC3);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC3));	 //等待校准结束
}		 
//获得ADC3某个通道的值
//ch:通道值 0~16
//返回值:转换结果
u16 Get_Adc3(u8 ch)   
{
  //设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC3, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC3,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC3, ENABLE);		//使能指定的ADC3的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC3, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC3);	//返回最近一次ADC3规则组的转换结果
} 



