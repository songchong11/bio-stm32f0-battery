#include "main.h"
#include "sys.h"
#include "delay.h"
#include "led.h"



void LED_Init(void);

//show you a blink
int main(void)
{
  LED_Init();
  delay_init();
  while (1)
  {
		LED_G=1;
		LED_B=1;

		delay_ms(500);
		LED_G=0;
	  	LED_B=0;

		delay_ms(500);
  }
}

