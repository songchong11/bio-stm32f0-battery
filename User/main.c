#include "main.h"
#include "sys.h"
#include "delay.h"
#include "led.h"
#include "usart.h"
#include "mb.h"
#include "timer.h"
#include "adc.h"

uint8_t slave_address = 0x00;

uint16_t adc_0_value, adc_1_value, adc_2_value;

float volt_0, volt_1, volt_2;
extern uint16_t RegularConvData_Tab[4];
extern uint8_t sample_finish;
extern uint16_t adc_value[5];  

//show you a blink
int main(void)
{

    LED_Init();
	slave_addr_gpio_Init();
	Adc_Init(); 
    delay_init();

	TIM14_Int_Init(10000, 4799);//1s: TIM14 is  a 48MHZ timer
	
	slave_address = get_slave_addr();

	mb_Modbus_Init();

	printf("slave_address is %d\r\n\r\n", slave_address);
	
	printf("bio-battery init ok start main...\r\n");

    while (1)
    {
		user_mb_app();
#if 0
		adc_0_value = Get_Adc_Average(ADC_Channel_5,10);//获取通道5的转换值，20次取平均
		adc_1_value = Get_Adc_Average(ADC_Channel_6,10);
		adc_2_value = Get_Adc_Average(ADC_Channel_7,10);


		volt_0 = (float)adc_0_value*(3.3/4096);
		volt_1 = (float)adc_1_value*(3.3/4096);
		volt_2 = (float)adc_2_value*(3.3/4096);

		printf(" volt: %f, %f, %f \r\n", volt_0, volt_1, volt_2);
#endif
		//printf(" %x, %x %x  %x\r\n", RegularConvData_Tab[0], RegularConvData_Tab[1],RegularConvData_Tab[2],RegularConvData_Tab[3]);
		//printf(" sample_finish %x \r\n", sample_finish); 
        if (sample_finish == 1) {
           printf(" %x %x %x %x %x\r\n", adc_value[0], adc_value[1], adc_value[2], adc_value[3], adc_value[4]); 
			 TIM_Cmd(TIM3, ENABLE);                     //完成周波采样，停止定时器  
            DMA_Cmd(DMA1_Channel1, ENABLE);            //完成周波采样，停止DMA  
            sample_finish = 0; 
        }
	}
	
}

