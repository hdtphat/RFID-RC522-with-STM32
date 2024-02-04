#ifndef MFRC522_H
#define MFRC522_H

#define     Reserved00            		0x00
#define     CommandReg            	0x01
#define     CommIEnReg            	0x02
#define     DivlEnReg             		0x03
#define     CommIrqReg            	0x04
#define     DivIrqReg             		0x05
#define     ErrorReg              		0x06
#define     Status1Reg            		0x07
#define     Status2Reg            		0x08
#define     FIFODataReg           	0x09
#define     FIFOLevelReg          		0x0A
#define     WaterLevelReg         	0x0B
#define     ControlReg            		0x0C
#define     BitFramingReg         	0x0D
#define     CollReg               		0x0E
#define     Reserved01            		0x0F
#define     Reserved10            		0x10
#define     ModeReg               	0x11
#define     TxModeReg             	0x12
#define     RxModeReg             	0x13
#define     TxControlReg          	0x14
#define     TxAutoReg             		0x15
#define     TxSelReg              		0x16
#define     RxSelReg              		0x17
#define     RxThresholdReg        	0x18
#define     DemodReg              	0x19
#define     Reserved11            		0x1A
#define     Reserved12            		0x1B
#define     MifareReg             		0x1C
#define     Reserved13            		0x1D
#define     Reserved14            		0x1E
#define     SerialSpeedReg        	0x1F
#define     Reserved20            		0x20
#define     CRCResultRegM         	0x21
#define     CRCResultRegL        	0x22
#define     Reserved21            		0x23
#define     ModWidthReg           	0x24
#define     Reserved22            		0x25
#define     RFCfgReg              		0x26
#define     GsNReg                		0x27
#define     CWGsPReg              	0x28
#define     ModGsPReg             	0x29
#define     TModeReg              	0x2A
#define     TPrescalerReg         		0x2B
#define     TReloadRegH           	0x2C
#define     TReloadRegL           	0x2D
#define     TCounterValueRegH    	0x2E
#define     TCounterValueRegL		0x2F
#define     Reserved30            		0x30
#define     TestSel1Reg           		0x31
#define     TestSel2Reg           		0x32
#define     TestPinEnReg          	0x33
#define     TestPinValueReg       	0x34
#define     TestBusReg            		0x35
#define     AutoTestReg           	0x36
#define     VersionReg            		0x37
#define     AnalogTestReg         	0x38
#define     TestDAC1Reg           	0x39
#define     TestDAC2Reg          	0x3A
#define     TestADCReg            	0x3B
#define     Reserved31            		0x3C
#define     Reserved32            		0x3D
#define     Reserved33            		0x3E
#define     Reserved34            		0x3F

typedef enum {
	MI_OK = 0,
	MI_NOTAGERR, // 1
	MI_ERR // 2
} MFRC522_Status_t;

#define MFRC522_DUMMY					0x00
#define MFRC522_MAX_LEN					16

/*=============================================================================================================================*/

#define PCD_IDLE						0x00   //NO action; Cancel the current command
#define PCD_AUTHENT				0x0E   //Authentication Key
#define PCD_RECEIVE					0x08   //Receive Data
#define PCD_TRANSMIT				0x04   //Transmit data
#define PCD_TRANSCEIVE			0x0C   //Transmit and receive data,
#define PCD_RESETPHASE			0x0F   //Reset
#define PCD_CALCCRC				0x03   //CRC Calculate
#define PICC_REQIDL					0x26   // find the antenna area does not enter hibernation
#define PICC_REQALL					0x52   // find all the cards antenna area
#define PICC_ANTICOLL				0x93   // anti-collision
#define PICC_SElECTTAG				0x93   // election card
#define PICC_AUTHENT1A			0x60   // authentication key A
#define PICC_AUTHENT1B			0x61   // authentication key B
#define PICC_READ						0x30   // Read Block
#define PICC_WRITE					0xA0   // write block
#define PICC_DECREMENT			0xC0   // debit
#define PICC_INCREMENT			0xC1   // recharge
#define PICC_RESTORE				0xC2   // transfer block data to the buffer
#define PICC_TRANSFER				0xB0   // save the data in the buffer
#define PICC_HALT						0x50   // Sleep

/*=============================================================================================================================*/

// MFRC522 interact functions
void MFRC522_WriteToRegister (uint8_t reg_address, uint8_t *data_array, uint8_t data_size);
void MFRC522_ReadFromRegister (uint8_t reg_address, uint8_t *RxBuffer_array, uint8_t data_size);
void MFRC522_SetBitMask (uint8_t reg_address, uint8_t mask);
void MFRC522_ResetBitMask (uint8_t reg_address, uint8_t mask);

// MFRC522 initialize functions
void MFRC522_Init(void);

// MFRC522 command functions
void MFRC522_AntennaOn(void);
void MFRC522_AntennaOff(void);
void MFRC522_Reset (void);

MFRC522_Status_t MFRC522_ToCard(uint8_t command, uint8_t* sendData, uint8_t sendLen, uint8_t* backData, uint16_t* backLen);
MFRC522_Status_t MFRC522_Request(uint8_t reqMode, uint8_t* TagType);
MFRC522_Status_t MFRC522_Anticoll(uint8_t* serNum);
void MFRC522_CalculateCRC(uint8_t*  pIndata, uint8_t len, uint8_t* pOutData);
void MFRC522_Halt(void) ;
MFRC522_Status_t MFRC522_Check(uint8_t* id);


#endif