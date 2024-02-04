#include "stm32f4xx.h" 
#include "SPI1.h"
#include <stdint.h>

void SPI1_port(void){
/*
	===================
	||	SPI1_NSS			PA4		||
	||	SPI1_SCLK			PA5		||
	||	SPI1_MISO			PA6		||
	||	SPI1_MOSI			PA7		||
	===================
*/	
	// Enable GPIO Clock
	RCC->AHB1ENR |= (1<<0);  	
	// Alternate functions for PA5, PA6, PA7 and Output for PA4
	GPIOA->MODER |= (2<<10)|(2<<12)|(2<<14)|(1<<8);  	
	// HIGH Speed for PA5, PA6, PA7, PA9	
	GPIOA->OSPEEDR |= (3<<10)|(3<<12)|(3<<14)|(3<<8);  	
	// AF5(SPI1) for PA5, PA6, PA7
	GPIOA->AFR[0] |= (5<<20)|(5<<24)|(5<<28);   					
}

void SPI1_setup(void){
/*
	// Enable SPI1 CLock
	=================
	||	STEPS TO FOLLOW 		||
	||									||
	||	1. Enable SPI clock		||
	||	2. Configure the CR1	||
	||	3. Configure the CR2	||
	=================
*/	
	RCC->APB2ENR |= (1U<<12);  	
	// CPOL= 0
	SPI1->CR1 &= ~(1U<<0);   
	// CPHA= 0	
	SPI1->CR1 &= ~(1U<<1);	
	// MSTR = 1 -> Master Mode	
	SPI1->CR1 |= (1U<<2);  
	// BR[2:0] = 011: fPCLK/16 -> PCLK2 = 84MHz, SPI1 clk (Baudrate) = 5.25MHz	
	SPI1->CR1 |= (3U<<3);  
	// LSBFIRST = 0 -> MSB first	
	SPI1->CR1 &= ~(1U<<7);  	
	// SSM=1, SSi=1 -> Software Slave Management	
	SPI1->CR1 |= (1U<<8) | (1U<<9);  
	// RXONLY = 0 -> full-duplex	
	SPI1->CR1 &= ~(1U<<10);  		
	// DFF=0 -> 8 bit data frame	
	SPI1->CR1 &= ~(1U<<11);  				
	SPI1->CR2 = 0;
}

void SPI1_enable (void){
	// SPE=1, Peripheral enabled
	SPI1->CR1 |= (1U<<6);   					
}

void SPI1_disable (void){
	// SPE=0, Peripheral Disabled
	SPI1->CR1 &= ~(1U<<6);   
}

void SPI1_NSSenable (void){
	GPIOA->BSRR |= (1U<<4)<<16;
}

void SPI1_NSSdisable (void){
	GPIOA->BSRR |= (1U<<4);
}

void SPI1_transmit (uint8_t *Tx_data_array, int size){	
/*
	=======================================
	||							STEPS TO FOLLOW								||
	||																					||
	||	1. 	Wait for the TXE bit to set in the Status Register			||
	||	2. 	Write the data to the Data Register							||
	||	3. 	After the data has been transmitted, wait for the BSY 	||
	||	bit to reset in Status Register											||
	||	4. 	Clear the Overrun flag by reading DR and SR				||
	=======================================
*/		
	int i=0;
	while (i<size){
		SPI1_NSSenable();
		// wait for TXE bit to set -> This will indicate that the buffer is empty
		while (!((SPI1->SR)&(1<<1))) {} 
		// load the data into the Data Register
		SPI1->DR = Tx_data_array[i];  
		// increase i for later transmition
		i++;
		// wait for TXE bit to set -> This will indicate that the buffer is empty
		while (!((SPI1->SR)&(1<<1))) {}  
		// wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication	
		while (((SPI1->SR)&(1<<7))) {}  
		//  Clear the Overrun flag by reading DR and SR
		uint32_t temp = SPI1->DR;
		temp = SPI1->SR;
		SPI1_NSSdisable();
	}	
}

void SPI1_receive (uint8_t *RX_data_array, int size){
/*
	=====================================
	||						STEPS TO FOLLOW 								||
	||																				||
	||	1. Wait for the BSY bit to reset in Status Register			||
	||	2. Send some Dummy data before reading the DATA	||
	||	3. Wait for the RXNE bit to Set in the status Register		||
	||	4. Read data from Data Register								||
	=====================================
*/		
	while (size){
		// wait for BSY bit to Reset -> This will indicate that SPI is not busy in communication
		while (((SPI1->SR)&(1<<7))) {}
		// send dummy data as any SPI device can only send data after recive some
		SPI1->DR = 0;  
		// Wait for RXNE to set -> This will indicate that the Rx buffer is not empty
		while (!((SPI1->SR) &(1<<0))){}
		*RX_data_array++ = (SPI1->DR);
		size--;
	}	
}

void SPI1_TransmitSingleByte (uint8_t byte){
	// wait for TXE bit to set -> This will indicate that the buffer is empty
	while (!((SPI1->SR)&(1<<1))) {} 
	// load the data into the Data Register
	SPI1->DR = byte;  
	// wait for TXE bit to set -> make sure that the buffer is empty
	while (!((SPI1->SR)&(1<<1))) {}  
	// wait for BSY bit to Reset -> make sure that SPI is not busy in communication	
	while (((SPI1->SR)&(1<<7))) {}  
	// clear the Overrun flag by reading DR and SR
	uint32_t clear_flag = SPI1->DR;
	clear_flag = SPI1->SR;
}
