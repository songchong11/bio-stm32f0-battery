#include "main.h"
#include "sys.h"
#include "delay.h"
#include "led.h"
#include "usart.h"
#include "mb.h"

void LED_Init(void);

//show you a blink
int main(void)
{

    LED_Init();
    delay_init();
	//uart_init(115200);
	
	mb_Modbus_Init();
	
	printf("bio-battery init...\r\n");
	printf("start main\r\n");

    while (1)
    {  
		LED_G=1;
		LED_B=1;
		
		user_mb_app();
		
		delay_ms(200);
		LED_G=0;
	  	LED_B=0;

		delay_ms(200);
	}
}

