/*
 * RF24 namespaced constants
 *
 * MIT/X license © 2016 Daniel Lima <danielm@tinyhub.tk>
 */

namespace rf24 {

// Registers
const uint8_t CONFIG      = 0x00;
const uint8_t EN_AA       = 0x01;
const uint8_t EN_RXADDR   = 0x02;
const uint8_t SETUP_AW    = 0x03;
const uint8_t SETUP_RETR  = 0x04;
const uint8_t RF_CH       = 0x05;
const uint8_t RF_SETUP    = 0x06;
const uint8_t NRF_STATUS  = 0x07;
const uint8_t OBSERVE_TX  = 0x08;
const uint8_t CD          = 0x09;
const uint8_t RX_ADDR_P0  = 0x0A;
const uint8_t RX_ADDR_P1  = 0x0B;
const uint8_t RX_ADDR_P2  = 0x0C;
const uint8_t RX_ADDR_P3  = 0x0D;
const uint8_t RX_ADDR_P4  = 0x0E;
const uint8_t RX_ADDR_P5  = 0x0F;
const uint8_t TX_ADDR     = 0x10;
const uint8_t RX_PW_P0    = 0x11;
const uint8_t RX_PW_P1    = 0x12;
const uint8_t RX_PW_P2    = 0x13;
const uint8_t RX_PW_P3    = 0x14;
const uint8_t RX_PW_P4    = 0x15;
const uint8_t RX_PW_P5    = 0x16;
const uint8_t FIFO_STATUS = 0x17;
const uint8_t DYNPD       = 0x1C;
const uint8_t FEATURE     = 0x1D;

// Bit Mnemonics
const uint8_t MASK_RX_DR  = 6;
const uint8_t MASK_TX_DS  = 5;
const uint8_t MASK_MAX_RT = 4;
const uint8_t EN_CRC      = 3;
const uint8_t CRCO        = 2;
const uint8_t PWR_UP      = 1;
const uint8_t PRIM_RX     = 0;


const uint8_t ENAA_P5     = 5;
const uint8_t ENAA_P4     = 4;
const uint8_t ENAA_P3     = 3;
const uint8_t ENAA_P2     = 2;
const uint8_t ENAA_P1     = 1;
const uint8_t ENAA_P0     = 0;


const uint8_t ERX_P5      = 5;
const uint8_t ERX_P4      = 4;
const uint8_t ERX_P3      = 3;
const uint8_t ERX_P2      = 2;
const uint8_t ERX_P1      = 1;
const uint8_t ERX_P0      = 0;

const uint8_t AW          = 0;
const uint8_t ARD         = 4;
const uint8_t ARC         = 0;
const uint8_t PLL_LOCK    = 4;
const uint8_t RF_DR       = 3;
const uint8_t RF_PWR      = 6;
const uint8_t RX_DR       = 6;
const uint8_t TX_DS       = 5;
const uint8_t MAX_RT      = 4;
const uint8_t RX_P_NO     = 1;
const uint8_t TX_FULL     = 0;
const uint8_t PLOS_CNT    = 4;
const uint8_t ARC_CNT     = 0;
const uint8_t TX_REUSE    = 6;
const uint8_t FIFO_FULL   = 5;
const uint8_t TX_EMPTY    = 4;
const uint8_t RX_FULL     = 1;
const uint8_t RX_EMPTY    = 0;
const uint8_t DPL_P5      = 5;
const uint8_t DPL_P4      = 4;
const uint8_t DPL_P3      = 3;
const uint8_t DPL_P2      = 2;
const uint8_t DPL_P1      = 1;
const uint8_t DPL_P0      = 0;
const uint8_t EN_DPL      = 2;
const uint8_t EN_ACK_PAY  = 1;
const uint8_t EN_DYN_ACK  = 0;

// SPI Commands
const uint8_t R_REGISTER    = 0x00;
const uint8_t W_REGISTER    = 0x20;
const uint8_t REGISTER_MASK = 0x1F;
const uint8_t ACTIVATE      = 0x50;
const uint8_t R_RX_PL_WID   = 0x60;
const uint8_t R_RX_PAYLOAD  = 0x61;
const uint8_t W_TX_PAYLOAD  = 0xA0;
const uint8_t W_ACK_PAYLOAD = 0xA8;
const uint8_t FLUSH_TX      = 0xE1;
const uint8_t FLUSH_RX      = 0xE2;
const uint8_t REUSE_TX_PL   = 0xE3;
const uint8_t NOP           = 0xFF;

// Extra mnemonics nRF24L01(+)
const uint8_t LNA_HCURR   = 0;
const uint8_t RPD         = 0x09;
const uint8_t W_TX_PAYLOAD_NO_ACK  = 0xB0;
const uint8_t RF_DR_LOW   = 5;
const uint8_t RF_DR_HIGH  = 3;
const uint8_t RF_PWR_LOW  = 1;
const uint8_t RF_PWR_HIGH = 2;

const uint8_t MASK_RF_DR  = bit(RF_DR_HIGH) | bit(RF_DR_LOW);
const uint8_t MASK_RF_PWR = bit(RF_PWR_LOW) | bit(RF_PWR_HIGH);

} // namespace rf24
