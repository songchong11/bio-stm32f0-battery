#include "main.h"
#include "sys.h"
#include "delay.h"
#include "led.h"
#include "usart.h"
#include "mb.h"
#include "timer.h"

uint8_t slave_address = 0x00;

//show you a blink
int main(void)
{

    LED_Init();
	slave_addr_gpio_Init();

    delay_init();

	TIM14_Int_Init(10000, 4799);//1s: TIM14 is  a 48MHZ timer
	slave_address = get_slave_addr();

	mb_Modbus_Init();

	printf("slave_address is %d\r\n\r\n", slave_address);
	
	printf("bio-battery init ok start main...\r\n");

    while (1)
    {
		user_mb_app();

	}
}

