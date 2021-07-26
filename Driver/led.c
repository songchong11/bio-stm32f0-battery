#include "led.h"
#include "stm32f0xx.h"
#include "usart.h"
#include "adc.h"

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


void slave_addr_gpio_Init(void)
{

	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);	 //使能PA端口时钟	
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;				 //LED0-->PB.5 端口配置
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 		 //输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	 GPIO_InitStructure.GPIO_PuPd= GPIO_PuPd_UP;//无上下拉
	 GPIO_Init(GPIOB, &GPIO_InitStructure);

}

/*
*
* addr is: B1 B0
*
*/
unsigned char get_slave_addr(void)
{
	unsigned char addr_B0 = 0, addr_B1 = 0;

	addr_B0 = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0);
	addr_B1 = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1);

	return (addr_B1 * 2 + addr_B0);
}


/**************************EXIT KEY **************************************************/
void EXIT_KEY_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStruct; 
		EXTI_InitTypeDef EXTI_InitStruct;
		NVIC_InitTypeDef NVIC_InitStruct;
		/* config the extiline(PB0) clock and AFIO clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

		/* Configyre P[A|B|C|D|E]0  NIVC  */
		NVIC_InitStruct.NVIC_IRQChannel = EXTI4_15_IRQn;
		NVIC_InitStruct.NVIC_IRQChannelPriority = 0x00;
		NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStruct);

		/* EXTI line gpio config(PF7) */	
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12; 
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; // 上拉输入
		GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* EXTI line(PB0) mode config */
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource12);
		EXTI_InitStruct.EXTI_Line = EXTI_Line12;
		EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
		EXTI_InitStruct.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStruct); 
}

void EXTI4_15_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line12) != RESET)
  {

	printf("key\r\n");
	start_once_a_time_adc_test();
    /* Clear the EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line12);
  }
}
/**************************EXIT KEY END**************************************************/
