 #include "led.h"
 #include "stm32f0xx.h"


void LED_Init(void){
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);	 //ʹ��PA�˿�ʱ��	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;				 //LED0-->PB.5 �˿�����
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; 		 //���
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	 GPIO_InitStructure.GPIO_OType= GPIO_OType_PP;//����
	 GPIO_InitStructure.GPIO_PuPd= GPIO_PuPd_NOPULL;//��������
	 GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);	 //ʹ��PA�˿�ʱ��	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;				 //LED0-->PB.5 �˿�����
	 GPIO_Init(GPIOA, &GPIO_InitStructure);	

		
	 GPIO_SetBits(GPIOA,GPIO_Pin_15);						 //PA.15 �����
	 GPIO_SetBits(GPIOB,GPIO_Pin_3);

}
	 
