#ifndef __led_H
#define __led_H 

#include "stm32f0xx.h"
#include "sys.h"

#define LED_G_PORT       GPIOB

//使用结构体的位段操作, 兼容Cortex-M3的位带操作.
#define LED_G_PORT_OUT    ((Bits_16_TypeDef *)(&(LED_G_PORT->ODR)))
//#define LED_G             (LED_G_PORT_OUT->bit3)    


#define LED_B_PORT       GPIOA

//使用结构体的位段操作, 兼容Cortex-M3的位带操作.
#define LED_B_PORT_OUT    ((Bits_16_TypeDef *)(&(LED_B_PORT->ODR)))
//#define LED_B             (LED_B_PORT_OUT->bit15)    


#define LED_G PBout(3)
#define LED_B PAout(15)

void LED_Init(void);

#endif
