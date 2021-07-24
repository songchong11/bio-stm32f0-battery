#ifndef __SYS_H
#define __SYS_H

#include "stm32f0xx.h"

//����GPIO�ṹ��λ��
typedef struct
{
    unsigned short bit0 : 1;
    unsigned short bit1 : 1;
    unsigned short bit2 : 1;
    unsigned short bit3 : 1;
    unsigned short bit4 : 1;
    unsigned short bit5 : 1;
    unsigned short bit6 : 1;
    unsigned short bit7 : 1;
    unsigned short bit8 : 1;
    unsigned short bit9 : 1;
    unsigned short bit10 : 1;
    unsigned short bit11 : 1;
    unsigned short bit12 : 1;
    unsigned short bit13 : 1;
    unsigned short bit14 : 1;
    unsigned short bit15 : 1;

} GPIO_Bit_TypeDef;


//&(GPIOX->ODR)��&(GPIOX->IDR)ǿ��ת��ΪGPIO_Bit_TypeDef* ָ�룬�ñ�ʶ��PORTX_OUT��PORTX_IN��ʾ
#define PORTA_OUT    ((GPIO_Bit_TypeDef *)(&(GPIOA->ODR)))
#define PORTA_IN     ((GPIO_Bit_TypeDef *)(&(GPIOA->IDR)))
#define PORTB_OUT    ((GPIO_Bit_TypeDef *)(&(GPIOB->ODR)))
#define PORTB_IN     ((GPIO_Bit_TypeDef *)(&(GPIOB->IDR)))
#define PORTC_OUT    ((GPIO_Bit_TypeDef *)(&(GPIOC->ODR)))
#define PORTC_IN     ((GPIO_Bit_TypeDef *)(&(GPIOC->IDR)))
#define PORTD_OUT    ((GPIO_Bit_TypeDef *)(&(GPIOD->ODR)))
#define PORTD_IN     ((GPIO_Bit_TypeDef *)(&(GPIOD->IDR)))
#define PORTE_OUT    ((GPIO_Bit_TypeDef *)(&(GPIOE->ODR)))
#define PORTE_IN     ((GPIO_Bit_TypeDef *)(&(GPIOE->IDR)))
#define PORTF_OUT    ((GPIO_Bit_TypeDef *)(&(GPIOF->ODR)))
#define PORTF_IN     ((GPIO_Bit_TypeDef *)(&(GPIOF->IDR)))
#define PORTG_OUT    ((GPIO_Bit_TypeDef *)(&(GPIOG->ODR)))
#define PORTG_IN     ((GPIO_Bit_TypeDef *)(&(GPIOG->IDR)))

//���ú꺯����##�ַ���ƴ�ӣ�ȷ��Ҫ������IO��
#define PAout(n) (PORTA_OUT->bit##n)
#define PAin(n)  (PORTA_IN->bit##n)
#define PBout(n) (PORTB_OUT->bit##n)
#define PBin(n)  (PORTB_IN->bit##n)
#define PCout(n) (PORTC_OUT->bit##n)
#define PCin(n)  (PORTC_IN->bit##n)
#define PDout(n) (PORTD_OUT->bit##n)
#define PDin(n)  (PORTD_IN->bit##n)
#define PEout(n) (PORTE_OUT->bit##n)
#define PEin(n)  (PORTE_IN->bit##n)
#define PFout(n) (PORTF_OUT->bit##n)
#define PFin(n)  (PORTF_IN->bit##n)
#define PGout(n) (PORTG_OUT->bit##n)
#define PGin(n)  (PORTG_IN->bit##n)

#define USE_CUSTOM_TYPE//ʹ���Զ������ͱ�������ʹ��ע�͵�

#ifdef USE_CUSTOM_TYPE

typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  /*!< Read Only */
typedef const int16_t sc16;  /*!< Read Only */
typedef const int8_t sc8;   /*!< Read Only */

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;  /*!< Read Only */
typedef __I int16_t vsc16;  /*!< Read Only */
typedef __I int8_t vsc8;   /*!< Read Only */

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  /*!< Read Only */
typedef const uint16_t uc16;  /*!< Read Only */
typedef const uint8_t uc8;   /*!< Read Only */

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;  /*!< Read Only */
typedef __I uint16_t vuc16;  /*!< Read Only */
typedef __I uint8_t vuc8;   /*!< Read Only */
#endif

#endif

