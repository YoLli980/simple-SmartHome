#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lcd.h"
#include "usart.h"	 
#include "adc.h"
#include "light.h"
#include "timer.h"
#include "dht11.h" 
#include "exti.h"
#include "ov7725.h"


  

#define START_TASK_PRIO 1
#define START_STK_SIZE 128
void start_task(void * pvParameters );	//任务函数
TaskHandle_t StartTask_Handler;			//任务句柄

//任务优先级
#define KEY_TASK_PRIO		2
//任务堆栈大小	
#define KEY_STK_SIZE 		128  
//任务句柄
TaskHandle_t KeyTask_Handler;
//任务函数
void key_task(void *pvParameters);


#define DHT11_TASK_PRIO 3
#define DHT11_STK_SIZE 128	
void DHT11_task(void * pvParameters );
TaskHandle_t DHT11_Handler;					//任务句柄


#define LSENSOR_TASK_PRIO 4
#define LSENSOR_STK_SIZE 128
void LSENSOR_task(void * pvParameters );
TaskHandle_t LSENSOR_Handler;					//任务句柄

#define CAMERA__TASK_PRIO 5
#define CAMERA__STK_SIZE 128
void CAMERA_task(void * pvParameters );
TaskHandle_t CAMERA__Handler;					//任务句柄




//传感器名字宏定义
#define  OV7725 1


//由于OV7725传感器安装方式原因,OV7725_WINDOW_WIDTH相当于LCD的高度，OV7725_WINDOW_HEIGHT相当于LCD的宽度
//注意：此宏定义只对OV7725有效
#define  OV7725_WINDOW_WIDTH		320 // <=320
#define  OV7725_WINDOW_HEIGHT		240 // <=240


const u8*LMODE_TBL[6]={"Auto","Sunny","Cloudy","Office","Home","Night"};//6种光照模式	    
const u8*EFFECTS_TBL[7]={"Normal","Negative","B&W","Redish","Greenish","Bluish","Antique"};	//7种特效 
extern u8 ov_sta;	//在exit.c里 面定义
extern u8 ov_frame;	//在timer.c里面定义 

void Hander_Init(void);								//各类函数初始化
void OV7725_camera_refresh(void);   	//更新LCD显示(OV7725)
void LCD_back(void);									//LCD背景墙

u8 MODE=0; 													//当前模式 0：正常模式   1：摄像模式



int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 
	Hander_Init();
	
	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (char*          )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}
 void start_task(void * pvParameters )
 {
	 
	 u8 key;
	 u8 tmp=0;					//判断是否处于正常模式
	 u8 tmp1=0;					//判断是否处于摄像头模式
	 
	 	//创建KEY任务
		xTaskCreate((TaskFunction_t )key_task,             
                (const char*    )"key_task",           
                (uint16_t       )KEY_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )KEY_TASK_PRIO,        
                (TaskHandle_t*  )&KeyTask_Handler);  
	  //创建温湿度任务
	  xTaskCreate((TaskFunction_t )DHT11_task,           			 //任务函数
                (char*          )"DHT11_task",         			 //任务名称
                (uint16_t       )DHT11_STK_SIZE,       			 //任务堆栈大小
                (void*          )NULL,                 			 //传递给任务函数的参数
                (UBaseType_t    )DHT11_TASK_PRIO,      			 //任务优先级
                (TaskHandle_t*  )&DHT11_Handler);  					 //任务句柄              
    
		//创建亮度调节任务
		xTaskCreate((TaskFunction_t ) LSENSOR_task,           	 //任务函数
									(char*          )"LSENSOR_task",           //任务名称
									(uint16_t       )LSENSOR_STK_SIZE,         //任务堆栈大小
									(void*          )NULL,                	   //传递给任务函数的参数
									(UBaseType_t    )LSENSOR_TASK_PRIO,        //任务优先级
									(TaskHandle_t*  )&LSENSOR_Handler);  		   //任务句柄  
		

		
    
		
	  vTaskDelete(StartTask_Handler);       									//NULL							
						
									
									
									
//	 	while(1)
//		{
//				
//					
//					key=KEY_Scan(0);
//					if(key==KEY0_PRES)
//					{
//							POINT_COLOR=RED;//设置字体为红色  	
//							//显示提示信息											      
//							LCD_ShowString(30,110,200,16,16,"CURRENT MODE:NOMAL");		
//							tmp = 0;
//							vTaskSuspend(CAMERA__Handler);	
//							vTaskResume(LSENSOR_Handler);
//							vTaskResume(DHT11_Handler);
//							
//							
//							
//							break;
//					}else if(key==KEY1_PRES)
//					{
//							POINT_COLOR=RED;//设置字体为红色  	
//							//显示提示信息		
//							tmp=1;						
//							LCD_ShowString(30,110,200,16,16,"CURRENT MODE:CAMERA");
//							vTaskResume(CAMERA__Handler);
//						  vTaskSuspend(LSENSOR_Handler);	
//							vTaskSuspend(DHT11_Handler);	
//							
//									
//							
//								
//							
//							break;
//					}
//					
//					delay_ms(5);
//		}
	 
	 
	 
	 


	
	 
 }

 
 //key任务函数
void key_task(void *pvParameters)
{
	u8 key,statflag=0;
	u8 statflag1=0;
	while(1)
	{
		key=KEY_Scan(0);
		switch(key)
		{
			case KEY1_PRES:
//				statflag=!statflag;
//				if(statflag==1)
//				{
//					//创建摄像头任务
					xTaskCreate((TaskFunction_t )  CAMERA_task,             //任务函数
                (char*          )"CAMERA_task",            	//任务名称
                (uint16_t       )CAMERA__STK_SIZE,     		  //任务堆栈大小
                (void*          )NULL,                  	 	//传递给任务函数的参数
                (UBaseType_t    )CAMERA__TASK_PRIO,      		//任务优先级
                (TaskHandle_t*  )&CAMERA__Handler);      		//任务句柄  
					
					printf("挂起任务1的运行!\r\n");
//				}
//				else if(statflag==0)
//				{
//					LCD_Clear(BLACK);		
//					vTaskResume(DHT11_Handler);	//恢复任务1
//					vTaskResume(LSENSOR_Handler);	//恢复任务1
//				}		
				
				break;
//			case KEY1_PRES:
//				statflag1=!statflag1;
//				if(statflag1==1)
//				{
//					LCD_Clear(LGRAY);
//					vTaskResume(CAMERA__Handler);	//恢复任务1
//					vTaskSuspend(LSENSOR_Handler);//挂起任务2
//					vTaskSuspend(DHT11_Handler);//挂起任务2
//				}
//				else if(statflag1==0)
//				{
//					LCD_Clear(LGRAY);
//					vTaskResume(LSENSOR_Handler);	//恢复任务1
//					vTaskResume(DHT11_Handler);	//恢复任务1
//					vTaskSuspend(CAMERA__Handler);//挂起任务
//				}	
//				
//				break;
		}
		vTaskDelay(10);			//延时10ms 
	}
}
 
 
 
 void DHT11_task(void * pvParameters )
 {
		u8 t=0;			    
		u8 temperature;  	    
		u8 humidity; 
	  delay_ms(10);
	  LCD_back();
	  
		
		while(DHT11_Init())	//DHT11初始化	
		{
				
				LCD_ShowString(30,150,200,16,16,"DHT11 Error");
				delay_ms(200);
				LCD_Fill(30,150,239,130+16,WHITE);
				delay_ms(200);
		}	
	
		
		while(1)
		{		
				
				if(t%10==0)			//每100ms读取一次
				{									  
					DHT11_Read_Data(&temperature,&humidity);	//读取温湿度值					    
					LCD_ShowNum(30+40,170,temperature,2,16);	//显示温度	   		   
					LCD_ShowNum(30+40,190,humidity,2,16);		//显示湿度	 	   
				}				   
				delay_ms(10);
				t++;
				if(t==20)
				{
					t=0;
				}
		}
 }

 void LSENSOR_task(void * pvParameters )
 {
	  u8 adcx; 
    POINT_COLOR=RED;//设置字体为红色  	
		
		LCD_ShowString(30,130,200,16,16,"LSENS_VAL:");
		while(1){
				adcx=Lsens_Get_Val();
				LCD_ShowxNum(30+10*8,130,adcx,3,16,0);//显示ADC的值 
				LED0=!LED0;
				delay_ms(10);	
				
				if(adcx <=15)
					TIM_SetCompare2(TIM3,300);	
				else if(adcx <=30)
					TIM_SetCompare2(TIM3,150);	
				else if(adcx <=50)
					TIM_SetCompare2(TIM3,130);	
				else if(adcx <=70)
					TIM_SetCompare2(TIM3,50);	
				else if(adcx <=90)
					TIM_SetCompare2(TIM3,30);
				else
					TIM_SetCompare2(TIM3,0);
		}
 }
 
 
 
 
  void CAMERA_task(void * pvParameters )
 {
		u8 sensor=0;
		u8 key;
		u8 i=0;	    
		u8 msgbuf[15];//消息缓存区
		u8 tm=0;
		u8 lightmode=0,effect=0;
		s8 saturation=0,brightness=0,contrast=0;
	  LCD_ShowString(30,210,200,16,16,"OV7725_OV7670 Init...");	
		
		while(1)
		{
	 
				if(OV7725_Init()==0)
				{
						sensor=OV7725;
						LCD_ShowString(30,210,200,16,16,"OV7725 Init OK       ");
//						while(1)
//						{
//								key=KEY_Scan(0);
//								if(key==KEY0_PRES)
//								{
//										OV7725_Window_Set(OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT,0);//QVGA模式输出
//										break;
//								}else if(key==KEY1_PRES)
//								{
//										OV7725_Window_Set(OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT,1);//VGA模式输出
//										break;
//								}
//								i++;
//								if(i==100)LCD_ShowString(30,230,210,16,16,"KEY0:QVGA  KEY1:VGA"); //闪烁显示提示信息
//								if(i==200)
//								{	
//										LCD_Fill(30,230,210,250+16,WHITE);
//										i=0; 
//								}
//								delay_ms(5);
//						}				
						OV7725_Light_Mode(lightmode);
						OV7725_Color_Saturation(saturation);
						OV7725_Brightness(brightness);
						OV7725_Contrast(contrast);
						OV7725_Special_Effects(effect);
						OV7725_CS=0;
						break;
				}else
				{
						LCD_ShowString(30,210,200,16,16,"OV7725 Error!!");
						delay_ms(200);
						LCD_Fill(30,210,239,246,WHITE);
						delay_ms(200);
				}
		}

			TIM6_Int_Init(10000,7199);			//10Khz计数频率,1秒钟中断									  
			EXTI8_Init();						//使能外部中断8,捕获帧中断			
			LCD_Clear(BLACK);						 	 
			while(1)
			{	
//						key=KEY_Scan(0);//不支持连按
//						if(key)
//						{
//								tm=20;
//								switch(key)
//								{				    
//										case KEY0_PRES:	//灯光模式Light Mode
//												lightmode++;
//												if(sensor==OV7725)
//												{
//														if(lightmode>5)lightmode=0;
//														OV7725_Light_Mode(lightmode);
//												}
//												sprintf((char*)msgbuf,"%s",LMODE_TBL[lightmode]);
//												break;
//												case KEY1_PRES:	//饱和度Saturation
//														 saturation++;
//												if(sensor==OV7725)
//												{
//														if(saturation>4)saturation=-4;
//														else if(saturation<-4)saturation=4;
//														OV7725_Color_Saturation(saturation);
//												}
//												sprintf((char*)msgbuf,"Saturation:%d",saturation);
//												break;
//												case WKUP_PRES:	//对比度Contrast			    
//															contrast++;
//												if(sensor==OV7725)
//												{
//														if(contrast>4)contrast=-4;
//														else if(contrast<-4)contrast=4;
//														OV7725_Contrast(contrast);
//												}
//												sprintf((char*)msgbuf,"Contrast:%d",contrast);
//												break;
//							}
//				}	 
				
				
						key=KEY_Scan(0);//不支持连按
						if(key)
						{
								tm=20;
								switch(key)
								{				    
										case KEY0_PRES:	
												OV7725_CS=0;
												LCD_back();
												vTaskResume(LSENSOR_Handler);	//恢复任务1
												vTaskResume(DHT11_Handler);	//恢复任务1
												vTaskDelete(CAMERA__Handler);       									//NULL	
														
												
											break;
									}
								}
				
				if(sensor==OV7725)OV7725_camera_refresh();		//更新显示
				
				if(tm)
				{
						LCD_ShowString((lcddev.width-240)/2+30,(lcddev.height-320)/2+60,200,16,16,msgbuf);
						tm--;
				}
				i++;
				if(i>=15)//DS0闪烁.
				{
						i=0;
						LED0=!LED0;
				}
			}	 
}  

 

 void Hander_Init(void)
{
		delay_init();	
		uart_init(115200);	 			//串口初始化为115200
		KEY_Init();								//按键IO初始化
		LED_Init();		  					//初始化与LED连接的硬件接口
		LCD_Init();								//初始化LCD
		Lsens_Init(); 						//初始化光敏传感器
		TIM3_PWM_Init(899,0);			 //不分频。PWM频率=72000000/900=80Khz
}


void OV7725_camera_refresh(void)
{
	u32 i,j;
 	u16 color;	 
	if(ov_sta)//有帧中断更新
	{
		LCD_Scan_Dir(U2D_L2R);		//从上到下,从左到右
		LCD_Set_Window((lcddev.width-OV7725_WINDOW_WIDTH)/2,(lcddev.height-OV7725_WINDOW_HEIGHT)/2,OV7725_WINDOW_WIDTH,OV7725_WINDOW_HEIGHT);//将显示区域设置到屏幕中央
		if(lcddev.id==0X1963)
			LCD_Set_Window((lcddev.width-OV7725_WINDOW_WIDTH)/2,(lcddev.height-OV7725_WINDOW_HEIGHT)/2,OV7725_WINDOW_HEIGHT,OV7725_WINDOW_WIDTH);//将显示区域设置到屏幕中央
		LCD_WriteRAM_Prepare();     //开始写入GRAM	
		OV7725_RRST=0;				//开始复位读指针 
		OV7725_RCK_L;
		OV7725_RCK_H;
		OV7725_RCK_L;
		OV7725_RRST=1;				//复位读指针结束 
		OV7725_RCK_H; 
		for(i=0;i<OV7725_WINDOW_HEIGHT;i++)
		{
			for(j=0;j<OV7725_WINDOW_WIDTH;j++)
			{
				OV7725_RCK_L;
				color=GPIOC->IDR&0XFF;	//读数据
				OV7725_RCK_H; 
				color<<=8;  
				OV7725_RCK_L;
				color|=GPIOC->IDR&0XFF;	//读数据
				OV7725_RCK_H; 
				LCD->LCD_RAM=color;  
			}
		}
 		ov_sta=0;					//清零帧中断标记
		ov_frame++; 
		LCD_Scan_Dir(DFT_SCAN_DIR);	//恢复默认扫描方向 
	} 
}

void LCD_back(void)
{
		
		LCD_Clear(BACK_COLOR);
		LCD_ShowString(30,150,200,16,16,"DHT11 OK");
		POINT_COLOR=BLUE;//设置字体为蓝色 
		LCD_ShowString(30,170,200,16,16,"Temp:  .C");	 
		LCD_ShowString(30,190,200,16,16,"Humi:  %");	
		POINT_COLOR=RED;//设置字体为红色  	
		//显示提示信息											      
		LCD_ShowString(30,50,200,16,16,"WelCome to SmartHome");	
		LCD_ShowString(30,70,200,16,16,"KEY0:NOMAL KEY1:CAMERA  ");	
		LCD_ShowString(30,90,200,16,16,"CAMERA :NO");
		LCD_ShowString(30,110,200,16,16,"2020/1/14");	  
		POINT_COLOR=BLUE;//设置字体为蓝色
		LCD_ShowString(30,130,200,16,16,"LSENS_VAL:");
}	


