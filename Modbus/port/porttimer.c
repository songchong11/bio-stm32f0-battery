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
			//通用定时器2中断初始化
			//这里时钟选择为APB1的2倍，而APB1为36M
			//arr：自动重装值。
			//psc：时钟预分频数
			//这里使用的是定时器3!
			TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
			NVIC_InitTypeDef NVIC_InitStructure;

			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
			
			//定时器TIM3初始化
			TIM_TimeBaseStructure.TIM_Period = usTim1Timerout50us - 1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
			TIM_TimeBaseStructure.TIM_Prescaler = 3600 - 1; //设置用来作为TIMx时钟频率除数的预分频值
			TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
			TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
			TIM_TimeBaseInit(MODBUS_TIM, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
		 
			TIM_ITConfig(MODBUS_TIM,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

			//中断优先级NVIC设置
			NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //从优先级3级
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
			NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


			TIM_Cmd(MODBUS_TIM, ENABLE);  //使能TIMx

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

void TIM2_IRQHandler(void)   //TIM2中断
{
	prvvModbusTIMISR();
}
