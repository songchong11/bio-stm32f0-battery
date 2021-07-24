 #include "led.h"
 #include "stm32f0xx.h"


void LED_Init(void){
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);	 //使能PA端口时钟	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				 //LED0-->PB.5 端口配置
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 		 //输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	 GPIO_InitStructure.GPIO_OType= GPIO_OType_PP;//推挽
	 GPIO_InitStructure.GPIO_PuPd= GPIO_PuPd_NOPULL;//无上下拉
	 GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);	 //使能PA端口时钟	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 //LED0-->PB.5 端口配置
	 GPIO_Init(GPIOA, &GPIO_InitStructure);	

		
	 GPIO_SetBits(GPIOA,GPIO_Pin_15);						 //PA.15 输出高
	 GPIO_SetBits(GPIOB,GPIO_Pin_3);

}
	 
