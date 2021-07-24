#include "main.h"
#include "sys.h"
#include "delay.h"

#define LED PBout(3)
void LED_Init(void);

//show you a blink
int main(void)
{
  LED_Init();
  delay_init();
  while (1)
  {
		LED=1;
		delay_ms(500);
		LED=0;
		delay_ms(500);
  }
}

