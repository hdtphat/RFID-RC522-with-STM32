#include "stm32f4xx.h"
#include "RCCconfig.h"
#include "BasicTimer.h"
#include "SPI1.h"
#include "MFRC522.h"
#include <stdint.h>

uint8_t cardID[5];
uint8_t num1, num2, num3, num4, num5;

int main(void){
	SystemClock_conf(); // system clock 168MHz
	TIM6_conf(); // delay_us and delay_ms
	SPI1_port();
	SPI1_setup();
	SPI1_enable();
	SPI1_NSSdisable();
	MFRC522_Init();
	MFRC522_AntennaOn();
	
		// Alternate functions for PA5, PA6, PA7 and Output for PA4
	GPIOA->MODER |= (1<<18);  	
	// HIGH Speed for PA5, PA6, PA7, PA9	
	GPIOA->OSPEEDR |= (3<<18);  
	
	while(1){

		if (MFRC522_Check(cardID) == MI_OK) {
			num1=cardID[0];
			num2=cardID[1];
			num3=cardID[2];
			num4=cardID[3];
			num5=cardID[4];
        }
	}
}
