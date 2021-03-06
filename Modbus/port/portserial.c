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

#define MODBUS_USART_CTRL_PORT		 GPIOA
#define MODBUS_USART_CTRL_PIN  		 GPIO_Pin_4

#define MODBUS_USART	USART1							

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

		RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOA, ENABLE);	//????USART1??GPIOA????
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE );
		
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_1);
		GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_1);

		/*
		*  USART1_TX -> PA2 , USART1_RX ->        PA3
		*/                                
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;                 
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(GPIOA, &GPIO_InitStructure); 

		//Usart1 NVIC ????
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPriority=3 ;//??????????3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ????????
		NVIC_Init(&NVIC_InitStructure);	//????????????????????VIC??????

		//USART ??????????

		USART_InitStructure.USART_BaudRate = ulBaudRate;//??????????
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//??????8??????????
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//??????????
		USART_InitStructure.USART_Parity = USART_Parity_No;//????????????
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//????????????????
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//????????

		USART_Init(MODBUS_USART, &USART_InitStructure); ; //??????????
		
		vMBPortSerialEnable(FALSE, FALSE);//free modbus
		
		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //????????2????
		NVIC_InitStructure.NVIC_IRQChannelPriority = 0; //??????????2??
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //????????????????
		NVIC_Init(&NVIC_InitStructure); //????NVIC_InitStruct??????????????????????NVIC??????
	 
		USART_ITConfig(MODBUS_USART, USART_IT_RXNE, ENABLE);//????????
		 
		USART_Cmd(MODBUS_USART, ENABLE);                    //???????? 
#if ENABLE_DEBUG_LOG
		printf("Modbus serial port init ok\r\n");
#endif
    return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */

		MODBUS_USART->TDR = ucByte; //
		//USART_SendData(MODBUS_USART, ucByte);

        while (USART_GetFlagStatus(MODBUS_USART,USART_FLAG_TXE) == RESET){} 
                  //??????????????????????????????????
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
		*pucByte = MODBUS_USART->RDR;
		//*pucByte = USART_ReceiveData(MODBUS_USART);
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


void USART1_IRQHandler(void)
{
	
	prvvModbusUARTISR();

} 

