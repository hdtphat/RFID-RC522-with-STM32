#include "stm32f4xx.h" 
#include "BasicTimer.h"
#include <stdint.h>

void TIM6_conf(void){
/*
	=======================================
	||	 						STEPS TO FOLLOW 								||
	||																					||
	||	1. Enable Timer clock													||
	||	2. Set the prescalar and the ARR									||
	||	3. Enable the Timer, and wait for the update Flag to set	||
	=======================================
	||						ADDITIONAL INFORMATION						||
	||																					||
	||	PCLK1 = APB1 = 42MHz (max)										||
	=======================================
*/
	//	1. Enable Timer clock
	RCC->APB1ENR |= (1U<<4);  					// Enable the timer6 clock
	
	//	2. Set the prescalar and the ARR
	TIM6->PSC = 84-1;  								// CK_CNT is equal to fCK_PSC / (PSC[15:0] + 1). -> 84MHz/84 = 1MHz ~ 1uS delay
	TIM6->ARR = 0xFFFF;  								// MAX ARR value -> counter will count to 2^16
	
	//	3. Enable the Timer, and wait for the update Flag to set
	TIM6->CR1 |= (1U<<0); 							// Enable the Counter
	while (!(TIM6->SR & (1U<<0)));  				// UIF: Update interrupt flag..  This bit is set by hardware when the registers are updated
}

void Delay_us (uint16_t time_us){
/*
	=========================================
	||								STEPS TO FOLLOW 								||
	||																						||
	||	1.	RESET the Counter														||
	||	2.	Wait for the Counter to reach the entered value. As each 	||
	||		count will take 1 us, the total waiting time will be the 		||
	||		required us delay														||
	=========================================
*/
	TIM6->CNT = 0;
	while (TIM6->CNT < time_us);
}

void Delay_ms (uint16_t time_ms){
	for (uint16_t i=0; i<time_ms; i++){
		Delay_us (1000); // delay of 1 ms
	}
}
