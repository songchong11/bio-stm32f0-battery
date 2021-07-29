#include "main.h"
#include "sys.h"
#include "delay.h"
#include "led.h"
#include "usart.h"
#include "mb.h"
#include "timer.h"
#include "adc.h"
#include <string.h>
#include "stm32f0xx_tim.h"


uint8_t slave_address = 0x00;

uint16_t adc_0_value, adc_1_value, adc_2_value;

volatile float volt_0, volt_1, volt_2;
extern uint16_t RegularConvData_Tab[4];
extern uint8_t sample_finish;
extern uint16_t adc_value[5]; 

extern uint16_t usRegHoldingBuf[REG_HOLDING_NREGS];
extern uint16_t usRegInputBuf[REG_INPUT_NREGS];

volatile int msg_type;

union volt{
	unsigned char volt_v[4];
	float volt_f;
};

static union volt voltage[3];

//show you a blink
int main(void)
{
    LED_Init();
	slave_addr_gpio_Init();
	Adc_Init(); 
    delay_init();
	EXIT_KEY_Init();
	//TIM14_Int_Init(10000, 4799);//1s: TIM14 is  a 48MHZ timer
	
	slave_address = get_slave_addr();

	mb_Modbus_Init();

	delay_ms(100);
	x9c103_gpio_init();


	printf("slave_address is %d\r\n\r\n", slave_address);
	
	printf("bio-battery init ok start main...\r\n");
	
#if 0
	//this is ok
	f_test.volt_f = 3.2999;
	f_test.volt_v[1] = 0x26;
	f_test.volt_v[0] = 0x00;
	f_test.volt_v[3] = 0x40;
	f_test.volt_v[2] = 0x53;
#endif


    while (1)
    {
		user_mb_app();
		
/*********************************** ADC measure ************************************************************************/
        if (sample_finish == 1) {
			//printf(" %x %x %x %x %x\r\n", adc_value[0], adc_value[1], adc_value[2], adc_value[3], adc_value[4]);

			LED_G = !LED_G;
			delay_ms(20);

			voltage[0].volt_f = (float)adc_value[2]*(3.3/4096);
			voltage[1].volt_f = (float)adc_value[3]*(3.3/4096);
			voltage[2].volt_f = (float)adc_value[4]*(3.3/4096);

			//printf(" %f %f %f\r\n", voltage[0].volt_f, voltage[1].volt_f, voltage[2].volt_f);
			memcpy(usRegInputBuf,	 &voltage[0].volt_v[0], 4);
			memcpy(&usRegInputBuf[2],&voltage[1].volt_v[0], 4);
			memcpy(&usRegInputBuf[4],&voltage[2].volt_v[0], 4);

			//printf(" %x %x %x %x %x %x\r\n\r\n",usRegInputBuf[0], usRegInputBuf[1], usRegInputBuf[2]\
										,usRegInputBuf[3], usRegInputBuf[4], usRegInputBuf[5]);

			//printf("--------- %f -----------\r\n",f_test.volt_f);
			//printf(" %x %x %x %x\r\n", f_test.volt_v[0], f_test.volt_v[1],f_test.volt_v[2],f_test.volt_v[3]);

			sample_finish = 0;
			TIM_Cmd(TIM3, ENABLE);                     //完成周波采样，停止定时器  
			DMA_Cmd(DMA1_Channel1, ENABLE);            //完成周波采样，停止DMA

        }
/*****************************WRITE holding reges***********************************************************************************/

		/*master write holding regs handler, up or down the wripe*/
		if (msg_type == MSG_WRITE_HOLD_REGS) {
			msg_type = NO_MSG;
			LED_B = 0;

			if(((usRegHoldingBuf[0] >> 8) & 0xff) == 0x01) {
				x9c103_wiper_up_or_down(usRegHoldingBuf[0] & 0xff , WRIPE_UP);

			} else if(((usRegHoldingBuf[0] >> 8) & 0xff) == 0x00) {
				x9c103_wiper_up_or_down(usRegHoldingBuf[0] & 0xff, WRIPE_DOWN);
			} else {
				//err code
			}
			usRegHoldingBuf[0] = 0x00;
			LED_B = 1;

		}
/********************************READ holding reges*****************************************************************************/
		if (msg_type == MSG_READ_HOLD_REGS) {
			msg_type = NO_MSG;

			LED_B = 0;
			delay_ms(20);
			LED_B = 1;
		}
		
/************************************************************************************************************************/
	}
	
}

