#ifndef __LSENS_H
#define __LSENS_H	
#include "sys.h" 
#include "adc.h" 

  
#define LSENS_READ_TIMES	10		//定义光敏传感器读取次数,读这么多次,然后取平均值
#define LSENS_ADC_CHX		ADC_Channel_6	//定义光敏传感器所在的ADC通道编号
    
void Lsens_Init(void); 				//初始化光敏传感器
u8 Lsens_Get_Val(void);				//读取光敏传感器的值
#endif 


