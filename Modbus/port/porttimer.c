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

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "sys.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR( void );

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortTimersInit( USHORT usTim1Timerout50us )
{
			//ͨ�ö�ʱ��2�жϳ�ʼ��
			//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
			//arr���Զ���װֵ��
			//psc��ʱ��Ԥ��Ƶ��
			//����ʹ�õ��Ƕ�ʱ��3!
			TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
			NVIC_InitTypeDef NVIC_InitStructure;

			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
			
			//��ʱ��TIM3��ʼ��
			TIM_TimeBaseStructure.TIM_Period = usTim1Timerout50us - 1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
			TIM_TimeBaseStructure.TIM_Prescaler = 3600 - 1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
			TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
			TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
			TIM_TimeBaseInit(MODBUS_TIM, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
		 
			TIM_ITConfig(MODBUS_TIM,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

			//�ж����ȼ�NVIC����
			NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
			NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


			TIM_Cmd(MODBUS_TIM, ENABLE);  //ʹ��TIMx

			return TRUE;
}


void vMBPortTimersEnable(  )
{
    /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
		TIM_ITConfig(MODBUS_TIM, TIM_IT_Update, DISABLE);
    TIM_Cmd(MODBUS_TIM, DISABLE);
    TIM_ClearITPendingBit(MODBUS_TIM, TIM_IT_Update);
    TIM_ClearFlag(MODBUS_TIM, TIM_FLAG_Update);
    TIM_SetCounter(MODBUS_TIM, 0);
    TIM_ITConfig(MODBUS_TIM, TIM_IT_Update, ENABLE);
    TIM_Cmd(MODBUS_TIM, ENABLE);
}

void vMBPortTimersDisable(  )
{
    /* Disable any pending timers. */
	 TIM_ClearITPendingBit(MODBUS_TIM, TIM_IT_Update);
   TIM_ITConfig(MODBUS_TIM, TIM_IT_Update, DISABLE);
	 TIM_SetCounter(MODBUS_TIM,0x0000); 
   TIM_Cmd(MODBUS_TIM, DISABLE);
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
static void prvvTIMERExpiredISR( void )
{
    ( void )pxMBPortCBTimerExpired(  );
}


void  prvvModbusTIMISR( void )
 {
    if(TIM_GetITStatus(MODBUS_TIM, TIM_IT_Update) == SET) {
      TIM_ClearITPendingBit(MODBUS_TIM, TIM_IT_Update);
      prvvTIMERExpiredISR();
   }
}

void TIM2_IRQHandler(void)   //TIM2�ж�
{
	prvvModbusTIMISR();
}
