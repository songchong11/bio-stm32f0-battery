/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

#include "port.h"
#include "sys.h"
#include "usart.h"	 

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

#define MODBUS_USART_CTRL_PORT		 GPIOB
#define MODBUS_USART_CTRL_PIN  		 GPIO_Pin_0

#define MODBUS_USART	USART2							

#define MODBUS_USART_TX_PIN 			GPIO_Pin_2
#define MODBUS_USART_RX_PIN 			GPIO_Pin_3

#define MODBUS_USART_TX_PORT			GPIOA
#define MODBUS_USART_RX_PORT			GPIOA


#define MODBUS_SEND()     (GPIO_SetBits(MODBUS_USART_CTRL_PORT, MODBUS_USART_CTRL_PIN))
#define MODBUS_RECIEVE()  (GPIO_ResetBits(MODBUS_USART_CTRL_PORT, MODBUS_USART_CTRL_PIN))

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
	    if(xRxEnable==TRUE) {
      MODBUS_RECIEVE();
      if(USART_GetFlagStatus(MODBUS_USART, USART_FLAG_RXNE) == SET) {
        USART_ClearFlag(MODBUS_USART, USART_FLAG_RXNE);
      }
      USART_ITConfig(MODBUS_USART, USART_IT_RXNE, ENABLE);
    } else if(xRxEnable == FALSE) {
      MODBUS_SEND();
      USART_ITConfig(MODBUS_USART, USART_IT_RXNE, DISABLE);
    }
    
    if(xTxEnable==TRUE) {
      MODBUS_SEND();
      if(USART_GetFlagStatus(MODBUS_USART, USART_FLAG_TC) == SET) {
        USART_ClearFlag(MODBUS_USART, USART_FLAG_TC);
      }
      USART_ITConfig(MODBUS_USART, USART_IT_TC, ENABLE);
    } else if(xTxEnable == FALSE) {
      MODBUS_RECIEVE();
      USART_ITConfig(MODBUS_USART, USART_IT_TC, DISABLE);
    }
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
	  GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
	 
		printf("Modbus serial port init\r\n");

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD, ENABLE);//使能GPIOA,D时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟
		
		GPIO_InitStructure.GPIO_Pin = MODBUS_USART_CTRL_PIN;	//RS485 CTRL PIN
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(MODBUS_USART_CTRL_PORT, &GPIO_InitStructure);
	 
		GPIO_InitStructure.GPIO_Pin = MODBUS_USART_TX_PIN;	//PA2
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
		GPIO_Init(MODBUS_USART_TX_PORT, &GPIO_InitStructure);
		 
		GPIO_InitStructure.GPIO_Pin = MODBUS_USART_RX_PIN;//PA3
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
		GPIO_Init(MODBUS_USART_RX_PORT, &GPIO_InitStructure);  

		RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,ENABLE);//复位串口2
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,DISABLE);//停止复位
	 
		
		USART_InitStructure.USART_BaudRate = ulBaudRate;//波特率设置
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据长度
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;///奇偶校验位
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式

		USART_Init(MODBUS_USART, &USART_InitStructure); ; //初始化串口
		
		vMBPortSerialEnable(FALSE, FALSE);//free modbus
		
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; //使能串口2中断
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //先占优先级2级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //从优先级2级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
		NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
	 
		USART_ITConfig(MODBUS_USART, USART_IT_RXNE, ENABLE);//开启中断
		 
		USART_Cmd(MODBUS_USART, ENABLE);                    //使能串口 
    return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */

		MODBUS_USART->DR = ucByte; //
		//USART_SendData(MODBUS_USART, ucByte);

        while (USART_GetFlagStatus(MODBUS_USART,USART_FLAG_TXE) == RESET){} 
                  //等待发送缓冲区空才能发送下一个字符
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
		*pucByte = MODBUS_USART->DR;
		//*pucByte = USART_ReceiveData(USART1);
    return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}

void  prvvModbusUARTISR( void )
{
	
	 if(USART_GetITStatus(MODBUS_USART, USART_IT_ORE) == SET) {
     USART_ClearITPendingBit(MODBUS_USART, USART_IT_ORE);
		 prvvUARTRxISR();
   }
		 
   if(USART_GetITStatus(MODBUS_USART, USART_IT_TC) == SET) {
     prvvUARTTxReadyISR();
     USART_ClearITPendingBit(MODBUS_USART, USART_IT_TC);
   }
 
   if(USART_GetITStatus(MODBUS_USART, USART_IT_RXNE) == SET) {
     prvvUARTRxISR();
     USART_ClearITPendingBit(MODBUS_USART, USART_IT_RXNE);
   }
}


void USART2_IRQHandler(void)
{
	
	prvvModbusUARTISR();

} 

