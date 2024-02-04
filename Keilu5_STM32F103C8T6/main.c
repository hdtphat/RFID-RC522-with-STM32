#include "stm32f10x.h"    
#include "delay.h"
#include "spi1.h"
#include "MFRC522.h"
#include <stdint.h>

uint8_t cardID[5];
uint8_t num1, num2, num3, num4, num5;

int main(void){
	SysTick_Config(9000);
	SPI1_InitPort();
	SPI1_Setup();
	SPI1_NSSdisable();
	SPI1_Enable();
	MFRC522_Init();
	MFRC522_AntennaOn();
	
	while(1){
		if (MFRC522_Check(cardID) == MI_OK) {
			num1=cardID[0];
			num2=cardID[1];
			num3=cardID[2];
			num4=cardID[3];
			num5=cardID[4];
			delay(100);
        }
	}
}


