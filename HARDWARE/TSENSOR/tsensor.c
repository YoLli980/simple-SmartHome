#include "tsensor.h"
#include "delay.h"
void T_Adc_Init(void)
{
		ADC_InitTypeDef ADC_InitStruce;
		
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA,ENABLE);
		RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
		ADC_DeInit(ADC1);
		ADC_InitStruce.ADC_ContinuousConvMode = DISABLE;
		ADC_InitStruce.ADC_DataAlign=ADC_DataAlign_Right;
		ADC_InitStruce.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
		
		ADC_InitStruce.ADC_Mode=ADC_Mode_Independent;
		ADC_InitStruce.ADC_NbrOfChannel = 1;
		ADC_InitStruce.ADC_ScanConvMode = DISABLE;
		ADC_Init(ADC1,&ADC_InitStruce);
	
		ADC_TempSensorVrefintCmd(ENABLE);					//	开启内部温度传感器
		ADC_Cmd(ADC1,ENABLE);
	  
		ADC_ResetCalibration(ADC1);
		while(ADC_GetResetCalibrationStatus(ADC1));
		ADC_StartCalibration(ADC1);
	  while(ADC_GetCalibrationStatus(ADC1));
	
}



u16  T_Get_Adc(u8 ch)
{
		ADC_RegularChannelConfig(ADC1,ch,1,ADC_SampleTime_239Cycles5);
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);
		while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
		return ADC_GetConversionValue(ADC1);

}
	




u16  T_Get_Adc_Average(u8 ch,u8 times)
{
		u32 temp_val=0;
		u8 t;
		for(t=0;t<times;t++)
		{	
				temp_val+=T_Get_Adc(ch);
				delay_ms(5);
		}
		return temp_val/times;
}


short Get_Temprate(void)
{
		u32 adcx;
		short result;
		double temperate;
		adcx = T_Get_Adc_Average(ADC_Channel_16,20);
		temperate=(float)adcx*(3.3/4096);
		temperate = (1.43-temperate)/0.0043+25;
		result=temperate*=100;
		return result;
		
	
}




