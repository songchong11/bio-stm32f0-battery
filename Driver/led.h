#ifndef __led_H
#define __led_H 

#include "stm32f0xx.h"
#include "sys.h"

#define LED_GPIO_CLK   RCC_AHBPeriph_GPIOA 
#define LED_PORT          GPIOA
#define LED_PIN        GPIO_Pin_4

//使用结构体的位段操作, 兼容Cortex-M3的位带操作.
#define LED_PORT_OUT    ((Bits_16_TypeDef *)(&(LED_PORT->ODR)))
#define LED               (LED_PORT_OUT->bit4)    

void LED_Init(void);

#endif
