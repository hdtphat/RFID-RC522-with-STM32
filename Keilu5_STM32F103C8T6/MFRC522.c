#include "stm32f10x.h"
#include "spi1.h"
#include "MFRC522.h"
#include <stdint.h>

void MFRC522_WriteToRegister (uint8_t reg_address, uint8_t *data_array, uint8_t data_size){ 
	/*
	||	To send data to any register of MFCR522:
	||	1.	Set NSS as low
	||	2.	Send 8bit address with following format:
	||		bit 7 (MSB) = 0 -> write mode
	||		bit6~1 = address
	||		bit0 (LSB) = 0 -> reverse
	||	3.	Sending data that needed to write into those register
	||	4.	Set NSS as high
	*/
	SPI1_NSSenable();
	reg_address = (reg_address<<1)&0x7E;
	SPI1_TransmitSingleByte(reg_address);
	SPI1_Transmit(data_array, data_size); 
	SPI1_NSSdisable();
}

void MFRC522_ReadFromRegister (uint8_t reg_address, uint8_t *RxBuffer_array, uint8_t data_size){
	/*
	||	To recieve some data from any register of MFCR522:
	||	1.	Set NSS as low
	||	2.	Send 8bit address with following format:
	||		bit 7 (MSB) = 1 -> read mode
	||		bit6~1 = address
	||		bit0 (LSB) = 0 -> reverse
	||	3.	Create a loop of:
	||		sending a byte dummy data (ex: 0x00) to recieve a byte of data
	||		store that byte of data into RxBuffer array
	||	4.	Set NSS as high
	*/
	SPI1_NSSenable();
	reg_address = ((reg_address<<1)&0x7E)|0x80;
	SPI1_TransmitSingleByte(reg_address);
	SPI1_Receive(RxBuffer_array, data_size);	
	SPI1_NSSdisable();
}

void MFRC522_SetBitMask (uint8_t reg_address, uint8_t mask)  {
	/*
	||	To set a bit of any register to 1:
	||	1.	Read 8bit data form that register
	||	2.	Store that byte of data into a temp variable
	||	2.	Set the target bit of that temp variable to 1
	||	3.	Send that new temp back to the register
	*/
	uint8_t temp[1] = {0x0};
	MFRC522_ReadFromRegister(reg_address, temp, 1);
	temp[0] |= (1U<<mask);
	MFRC522_WriteToRegister(reg_address, temp, 1);
}

void MFRC522_ResetBitMask (uint8_t reg_address, uint8_t mask)  {
	/*
	||	To reset a bit of any register to 0:
	||	1.	Read 8bit data form that register
	||	2.	Store that byte of data into a temp variable
	||	2.	Reset the target bit of that temp variable to 0
	||	3.	Send that new temp back to the register
	*/
	uint8_t temp[1] = {0x0};
	MFRC522_ReadFromRegister(reg_address, temp, 1);
	temp[0] &= ~(1U<<mask);
	MFRC522_WriteToRegister(reg_address, temp, 1);
}

/*==========================================================================================*/

void MFRC522_AntennaOn(void){
	uint8_t temp;
	MFRC522_ReadFromRegister(TxControlReg, &temp, 1);
	if (!(temp & 0x03)){
		// set bit0 and bit1 of TxControlReg to 1
		MFRC522_SetBitMask(TxControlReg, 0);
		MFRC522_SetBitMask(TxControlReg, 1);
	}
}

void MFRC522_AntennaOff(void){
	// Reset bit0 and bit1 of TxControlReg to 0
	MFRC522_ResetBitMask(TxControlReg, 0);
	MFRC522_ResetBitMask(TxControlReg, 1);
} 

void MFRC522_Reset (void){
	uint8_t CommandReg_data = 0x0F;
	MFRC522_WriteToRegister(CommandReg, &CommandReg_data, 1);
}

/*==========================================================================================*/

void MFRC522_Init(void){
	uint8_t TModeReg_data = 0x8D;
	uint8_t TPrescalerReg_data = 0x3E;
	uint8_t TReloadRegL_data = 0x30;
	uint8_t TReloadRegH_data = 0x00;
	uint8_t RFCfgReg_data = 0x70;
	uint8_t TxAutoReg_data = 0x40;	
	uint8_t ModeReg_data = 0x3D;
	uint8_t CommIEnReg_data = 0xFF;
	uint8_t DivlEnReg_data = 0xFF;
	
	MFRC522_Reset();
    // Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
    MFRC522_WriteToRegister(TModeReg, &TModeReg_data, 1);			// Tauto=1; f(Timer) = 6.78MHz/TPreScaler
    MFRC522_WriteToRegister(TPrescalerReg, &TPrescalerReg_data, 1);	// TModeReg[3..0] + TPrescalerReg
    MFRC522_WriteToRegister(TReloadRegL, &TReloadRegL_data, 1);
    MFRC522_WriteToRegister(TReloadRegH, &TReloadRegH_data, 1);
	MFRC522_WriteToRegister(RFCfgReg, &RFCfgReg_data, 1);
    MFRC522_WriteToRegister(TxAutoReg, &TxAutoReg_data, 1);     		// force 100% ASK modulation
    MFRC522_WriteToRegister(ModeReg, &ModeReg_data, 1); 			// CRC Initial value 0x6363
    // interrupts, still playing with these
    MFRC522_WriteToRegister(CommIEnReg, &CommIEnReg_data, 1);
    MFRC522_WriteToRegister(DivlEnReg, &DivlEnReg_data, 1);
    // turn antenna on
    MFRC522_AntennaOn();
}

MFRC522_Status_t MFRC522_ToCard(uint8_t command, uint8_t* sendData, uint8_t sendLen, uint8_t* backData, uint16_t* backLen) {
	MFRC522_Status_t status = MI_ERR;
	uint8_t irqEn = 0x00;
	uint8_t waitIRq = 0x00;
	uint8_t lastBits;
	uint16_t i;

	switch (command) {
		case PCD_AUTHENT: 
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		case PCD_TRANSCEIVE: 
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		default:
			break;
	}
	uint8_t CommIEnReg_data = irqEn | 0x80;
	MFRC522_WriteToRegister(CommIEnReg, &CommIEnReg_data,1);
	MFRC522_ResetBitMask(CommIrqReg, 7);
	MFRC522_SetBitMask(FIFOLevelReg, 7);
	
	uint8_t CommandReg_data = PCD_IDLE;
	MFRC522_WriteToRegister(CommandReg, &CommandReg_data, 1);

	//Writing data to the FIFO
	for (i = 0; i < sendLen; i++) { 
		uint8_t FIFODataReg_data = sendData[i];
		MFRC522_WriteToRegister(FIFODataReg, &FIFODataReg_data, 1);
	}

	//Execute the command
	CommandReg_data = command;
	MFRC522_WriteToRegister(CommandReg, &CommandReg_data, 1);
	if (command == PCD_TRANSCEIVE) {    
		MFRC522_SetBitMask(BitFramingReg, 7);		//StartSend=1,transmission of data starts
	}   

	//Waiting to receive data to complete
	i = 2000;	//i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms???
	uint8_t temp = 0;
	do {
		//CommIrqReg[7..0]
		//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
		MFRC522_ReadFromRegister(CommIrqReg, &temp, 1);
		i--;
	} while ((i!=0) && !(temp&0x01) && !(temp&waitIRq));

	MFRC522_ResetBitMask(BitFramingReg, 7);			//StartSend=0

	if (i != 0)  {
		uint8_t ErrorReg_data = 0;
		MFRC522_ReadFromRegister(ErrorReg, &ErrorReg_data, 1);
		if (!(ErrorReg_data & 0x1B)) {
			status = MI_OK;
			if (temp & irqEn & 0x01) {   
				status = MI_NOTAGERR;			
			}

			if (command == PCD_TRANSCEIVE) {
				MFRC522_ReadFromRegister(FIFOLevelReg, &temp, 1);
				uint8_t ControlReg_data = 0;
				MFRC522_ReadFromRegister(ControlReg, &ControlReg_data, 1);
				lastBits = ControlReg_data  & 0x07;
				if (lastBits) {   
					*backLen = (temp - 1) * 8 + lastBits;   
				} else {   
					*backLen = temp * 8;   
				}

				if (temp == 0) {   
					temp = 1;    
				}
				if (temp > MFRC522_MAX_LEN) {   
					temp = MFRC522_MAX_LEN;   
				}

				//Reading the received data in FIFO
				for (i = 0; i < temp; i++) {   
					 MFRC522_ReadFromRegister(FIFODataReg, &backData[i], 1);
				}
			}
		} else {   
			status = MI_ERR;  
		}
	}
	return status;
}

MFRC522_Status_t MFRC522_Request(uint8_t reqMode, uint8_t* TagType) {
	MFRC522_Status_t status;
	uint16_t backBits;			//The received data bits

	uint8_t BitFramingReg_data = 0x07;
	MFRC522_WriteToRegister(BitFramingReg, &BitFramingReg_data, 1);		//TxLastBists = BitFramingReg[2..0]	???

	TagType[0] = reqMode;
	status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

	if ((status != MI_OK) || (backBits != 0x10)) {    
		status = MI_ERR;
	}
	return status;
}

MFRC522_Status_t MFRC522_Anticoll(uint8_t* serNum) {
	MFRC522_Status_t status;
	uint8_t i;
	uint8_t serNumCheck = 0;
	uint16_t unLen;

	uint8_t BitFramingReg_data = 0x00;
	MFRC522_WriteToRegister(BitFramingReg, &BitFramingReg_data, 1);		//TxLastBists = BitFramingReg[2..0]

	serNum[0] = PICC_ANTICOLL;
	serNum[1] = 0x20;
	status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

	if (status == MI_OK) {
		//Check card serial number
		for (i = 0; i < 4; i++) {   
			serNumCheck ^= serNum[i];
		}
		if (serNumCheck != serNum[i]) {   
			status = MI_ERR;    
		}
	}
	return status;
}

void MFRC522_CalculateCRC(uint8_t*  pIndata, uint8_t len, uint8_t* pOutData) {
	uint8_t i, n;

	MFRC522_ResetBitMask(DivIrqReg, 2);				//CRCIrq = 0
	MFRC522_SetBitMask(FIFOLevelReg, 7);			//Clear the FIFO pointer
	//Write_MFRC522(CommandReg, PCD_IDLE);

	//Writing data to the FIFO	
	for (i = 0; i < len; i++) {   
		uint8_t FIFODataReg_data = pIndata[i];
		MFRC522_WriteToRegister(FIFODataReg, &FIFODataReg_data, 1);
	}
	uint8_t CommandReg_data = PCD_CALCCRC;
	MFRC522_WriteToRegister(CommandReg, &CommandReg_data, 1);

	//Wait CRC calculation is complete
	i = 0xFF;
	do {
		MFRC522_ReadFromRegister(DivIrqReg, &n, 1);
		i--;
	} while ((i!=0) && !(n&0x04));			//CRCIrq = 1

	//Read CRC calculation result
	uint8_t temp1, temp2;
	MFRC522_ReadFromRegister(CRCResultRegL, &temp1, 1);
	MFRC522_ReadFromRegister(CRCResultRegM, &temp2, 1);
	pOutData[0] = temp1;
	pOutData[1] = temp2;
}

void MFRC522_Halt(void) {
	uint16_t unLen;
	uint8_t buff[4]; 

	buff[0] = PICC_HALT;
	buff[1] = 0;
	MFRC522_CalculateCRC(buff, 2, &buff[2]);
	MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}

MFRC522_Status_t MFRC522_Check(uint8_t* id) {
	MFRC522_Status_t status;
	//Find cards, return card type
	status = MFRC522_Request(PICC_REQIDL, id);
	if (status == MI_OK) {
		//Card detected
		//Anti-collision, return card serial number 4 bytes
		status = MFRC522_Anticoll(id);
	}
	MFRC522_Halt();			//Command card into hibernation

	return status;
}
