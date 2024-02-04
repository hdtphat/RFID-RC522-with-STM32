#include "stm32f4xx.h"
#include "RCCconfig.h"
#include <stdint.h>

void SystemClock_conf(void){
/*
	================================================
	||									STEPS TO FOLLOW											||
	||																										||
	||	1. ENABLE HSE and wait for the HSE to become Ready							||
	||	2. Set the POWER ENABLE CLOCK and VOLTAGE REGULATOR				||
	||	3. Configure the FLASH PREFETCH and the LATENCY Related Settings		||
	||	4. Configure the PRESCALARS HCLK, PCLK1, PCLK2								||
	||	5. Configure the MAIN PLL																||
	||	6. Enable the PLL and wait for it to become ready								||
	||	7. Select the Clock Source and wait for it to be set								||
	||																										||
	||	=>	open clock.png to see description image of clock system				||
	================================================
	||							CLOCK FREQUENCY AFTER SETUP								||
	||																										||
	||	AHB = HCLK = 168MHz																	||
	||	PCLK1 = APB1 = 42MHz (max)															||
	||	PCLK2 = APB1 = 84MHz (max)															||
	================================================
*/
	//	1. ENABLE HSE and wait for the HSE to become Ready		
	RCC->CR |= (1U<<16);								// HSEON = 1 -> enable HSE clock as external crystal clock
	while( !(RCC->CR & (1U<<17)) ){}					// wait for flag HSERDY = 1 -> HSE is enable
	
	//	2. Set the POWER ENABLE CLOCK and VOLTAGE REGULATOR	
	RCC->APB1ENR |= (1U<<28);						// PWREN = 1 -> enable clock to APB1
	PWR->CR |= (3U<<14);								// VOS = 11 -> Scale 1 mode (STM32F42xx and STM32F43xx)
		
	//	3. Configure the FLASH PREFETCH and the LATENCY Related Settings	
	FLASH->ACR |= (1U<<9);								// ICEN = 1 -> Instruction cache is enabled (STM32F42xx and STM32F43xx)
	FLASH->ACR |= (1U<<10);							// DCEN = 1 -> Data cache is enabled (STM32F42xx and STM32F43xx)
	FLASH->ACR |= (1U<<8);								// PRFTEN = 1 -> Prefetch is enabled (STM32F42xx and STM32F43xx)
	FLASH->ACR |= (5U<<0);								// LATENCY = 101 -> ratio of the CPU clock period to the Flash memory access time: 5 wait state
		
	//	4. Configure the PRESCALARS HCLK, PCLK1, PCLK2	
	RCC->CFGR &= 0xFF0F;								// HPRE = 0 -> AHB prescaler divided by 1
	RCC->CFGR |= (5U<<10);								// PPRE1 = 101 -> APB1 prescaler divided by 4
	RCC->CFGR |= (4U<<13);								// PPRE2 = 100 -> APB2 prescaler divided by 2 
		
	//	5. Configure the MAIN PLL			
	RCC->PLLCFGR &= 0xFFFC0000;					//	clear all PLLM, PLLN and PLLP
	RCC->PLLCFGR |= (4U<<0);							//	PLLM = 4 -> /M prescaler divived by 4
	RCC->PLLCFGR |= (168U<<6);						//	PLLN = 168 -> *N prescaler multipied by 168
	RCC->PLLCFGR &= ~(3U<<16);					//	PLLP = 0 -> /P prescaler divived by 2
	RCC->PLLCFGR |= (1U<<22);						//	PLLSRC = 1 -> HSE oscillator clock selected as PLL and PLLI2S clock entry
	
	//	6. Enable the PLL and wait for it to become ready		
	RCC->CR |= (1U<<24);								//	PLLON = 1 -> enable PLL
	while( !(RCC->CR & (1U<<25)) ){}					//	wait for PLL to be enable
	
	//	7. Select the Clock Source and wait for it to be set	
	RCC->CFGR |= (2U<<0);								//	SW = 10 -> switch System clk switch into PLL input
	while( (RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL ){}
}
