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
		
	 GPIO_SetBits(GPIOB,GPIO_Pin_3);						 //PA.4 输出高

}
	 
