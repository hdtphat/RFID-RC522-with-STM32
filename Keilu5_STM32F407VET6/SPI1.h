#ifndef SPI1_H
#define SPI1_H

void SPI1_port(void);
void SPI1_setup(void);
void SPI1_enable (void);
void SPI1_disable (void);
void SPI1_NSSenable (void);
void SPI1_NSSdisable (void);
void SPI1_transmit (uint8_t *Tx_data_array, int size);
void SPI1_receive (uint8_t *RX_data_array, int size);
void SPI1_TransmitSingleByte (uint8_t byte);

#endif
