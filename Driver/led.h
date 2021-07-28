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

#define X9C103_INC 		PAout(8)
#define X9C103_UD 		PAout(9)
#define X9C103_CS		PAout(10)
#define WRIPE_UP 	1
#define WRIPE_DOWN  0

enum {
	NO_MSG,
	MSG_WRITE_HOLD_REGS,
	MSG_READ_HOLD_REGS,
	MSG_READ_INPUT_REGS,
};


void LED_Init(void);
void slave_addr_gpio_Init(void);
unsigned char get_slave_addr(void);
void EXIT_KEY_Init(void);
void x9c103_gpio_init(void);
int x9c103_wiper_up_or_down(uint8_t steps, uint8_t wripe_ud);
void x9c103_store_wripe_position(void);

#endif
