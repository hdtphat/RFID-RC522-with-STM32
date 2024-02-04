#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include <stdint.h>

volatile uint32_t msTicks = 0;

void SysTick_Handler(void){
	 /* Increment counter necessary in Delay()*/
		msTicks++;
}

void delay(uint32_t dlyTicks){
      uint32_t curTicks;
      curTicks = msTicks;
      while ((msTicks - curTicks) < dlyTicks) ;
}
