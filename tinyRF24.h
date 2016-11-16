/*
 * Minimalist RF24 library for Arduino.
 *
 * MIT/X license © 2016 Daniel Lima <danielm@tinyhub.tk>
 */

#pragma once

#include <SPI.h>
#include "nRF24L01.hh"

enum rf24_pa_dbm_enum {
	RF24_PA_MIN  = 0,
	RF24_PA_LOW  = 2,
	RF24_PA_HIGH = 4,
	RF24_PA_MAX  = 6,
	RF24_PA_ERROR
};

enum rf24_datarate_enum {
	RF24_1MBPS = 0,
	RF24_2MBPS = 8,
	RF24_250KBPS = 0x20
};

enum rf24_crclength_enum {
	RF24_CRC_DISABLED = 0,
	RF24_CRC_8  = 8,
	RF24_CRC_16 = 0xC
};

namespace rf24 {

template<uint8_t ce, uint8_t cs, uint8_t t_payload_size>
class RF24 {
public:
	void begin() {
		SPI.begin();
		pinMode(ce, OUTPUT);
		pinMode(cs, OUTPUT);
		digitalWrite(ce, LOW);
		digitalWrite(cs, HIGH);
		delayMicroseconds(5000);
		write_register(NRF_STATUS, bit(RX_DR) | bit(TX_DS) | bit(MAX_RT));
		write_register(SETUP_RETR, 0x3f);
		write_register(CONFIG, 0b1010);
		delayMicroseconds(1500);
	}

	void startListening() {
		spi(FLUSH_RX);
		write_register(CONFIG, read_register(CONFIG) | bit(PRIM_RX));
		digitalWrite(ce, HIGH);
		delayMicroseconds(130);
	}

	void stopListening() {
		digitalWrite(ce, LOW);
		delayMicroseconds(250);
		write_register(CONFIG, read_register(CONFIG) & ~bit(PRIM_RX));
	}

	bool write(const void *buf, uint8_t len) {
		spi(FLUSH_TX);
		spi(W_TX_PAYLOAD, buf, len);
		digitalWrite(ce, HIGH);
		delayMicroseconds(10);
		digitalWrite(ce, LOW);
		do {
			delayMicroseconds(200);
		} while (!(status() & (bit(TX_DS) | bit(MAX_RT))));
		uint8_t s = write_register(NRF_STATUS, bit(TX_DS) | bit(MAX_RT));
		return bitRead(s, TX_DS);
	}

	bool available() {
		return bitRead(status(), RX_DR) || !bitRead(read_register(FIFO_STATUS), RX_EMPTY);
	}

	bool available(uint8_t *pipe) {
		*pipe = (status() >> RX_P_NO) & 0b111;
		return available();
	}

	void read(void *buf, uint8_t len) {
		memset(buf, NOP, len);
		spi(R_RX_PAYLOAD, buf, len);
		write_register(NRF_STATUS, bit(RX_DR));
	}

	void openWritingPipe(uint64_t addr) {
		openWritingPipe(reinterpret_cast<const void*>(&addr));
	}

	void openWritingPipe(const void *addr) {
		spi(W_REGISTER | TX_ADDR, addr, addr_width);
		openReadingPipe(0, addr);
	}

	void openReadingPipe(uint8_t num, uint64_t addr) {
		openReadingPipe(num, reinterpret_cast<const void*>(&addr));
	}

	void openReadingPipe(uint8_t num, const void *addr) {
		spi(W_REGISTER | RX_ADDR_P0 + num, addr, addr_width);
		write_register(EN_RXADDR, read_register(EN_RXADDR) | bit(num));
		write_register(RX_PW_P0 + num, payload_size);
	}

	void setChannel(uint8_t channel) {
		write_register(RF_CH, channel);
	}

	void setPALevel(rf24_pa_dbm_enum level) {
		write_register(RF_SETUP, (read_register(RF_SETUP) & 0xF9) | level);
	}

	void setDataRate(rf24_datarate_enum rate) {
		write_register(RF_SETUP, (read_register(RF_SETUP) & 0xD7) | rate);
	}

	void setCRCLength(rf24_crclength_enum crclen) {
		write_register(CONFIG, (read_register(CONFIG) & 0xF3) | crclen);
	}

	void setPayloadSize(uint8_t size) {
		payload_size = max(1, min(32, size));
	}

	void setAddressWidth(uint8_t size) {
		addr_width = max(3, min(5, size));
		write_register(SETUP_AW, addr_width-2);
	}

	void setAutoAck(bool enable) {
		write_register(EN_AA, enable ? 0x3F : 0);
	}

	void setAutoAck(uint8_t pipe, bool enable) {
		uint8_t reg = read_register(EN_AA);
		if (enable) {
			reg |= bit(pipe);
		} else {
			reg &= ~bit(pipe);
		}
		write_register(EN_AA, reg);
	}

	void powerDown() {
		write_register(CONFIG, read_register(CONFIG) & ~bit(PWR_UP));
	}

	void powerUp() {
		write_register(CONFIG, read_register(CONFIG) | bit(PWR_UP));
		delayMicroseconds(1500);
	}

	void printDetails() {
	#if !ATtiny13
		char buf[64];
		uint8_t a1[5] = {0,0,0,0,0},
						a2[5] = {0,0,0,0,0};
		uint8_t s = status();
		sprintf(buf, "STATUS\t\t = 0x%02x RX_DR=%d TX_DS=%d MAX_RT=%d RX_P_NO=%d TX_FULL=%d",
				s, bitRead(s,6), bitRead(s,5), bitRead(s,4), (s>>1)&7, bitRead(s,0));
		Serial.println(buf);
		spi(R_REGISTER | RX_ADDR_P0, a1, addr_width);
		spi(R_REGISTER | RX_ADDR_P1, a2, addr_width);
		sprintf(buf, "RX_ADDR_P0-1\t = 0x");
		for (int i=0; i<addr_width; i++) {
			sprintf(buf+18+2*i, "%02x ", a1[addr_width-i-1]);
		}
		sprintf(buf+18+2*addr_width, " 0x");
		for (int i=0; i<addr_width; i++) {
			sprintf(buf+21+2*(addr_width+i), "%02x", a2[addr_width-i-1]);
		}
		Serial.println(buf);
		sprintf(buf, "RX_ADDR_P2-5\t = 0x%02x 0x%02x 0x%02x 0x%02x",
				read_register(RX_ADDR_P2), read_register(RX_ADDR_P3),
				read_register(RX_ADDR_P4), read_register(RX_ADDR_P5));
		Serial.println(buf);
		spi(R_REGISTER | TX_ADDR, a1, addr_width);
		sprintf(buf, "TX_ADDR\t\t = 0x");
		for (int i=0; i<addr_width; i++) {
			sprintf(buf+14+2*i, "%02x", a1[addr_width-i-1]);
		}
		Serial.println(buf);
		sprintf(buf, "RX_PW_P0-5\t = 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
				read_register(RX_PW_P0), read_register(RX_PW_P1), read_register(RX_PW_P2),
				read_register(RX_PW_P3), read_register(RX_PW_P4), read_register(RX_PW_P5));
		Serial.println(buf);
		sprintf(buf, "EN_AA\t\t = 0x%02x", read_register(EN_AA));
		Serial.println(buf);
		sprintf(buf, "EN_RXADDR\t = 0x%02x", read_register(EN_RXADDR));
		Serial.println(buf);
		sprintf(buf, "RF_CH\t\t = 0x%02x", read_register(RF_CH));
		Serial.println(buf);
		sprintf(buf, "RF_SETUP\t = 0x%02x", read_register(RF_SETUP));
		Serial.println(buf);
		sprintf(buf, "CONFIG\t\t = 0x%02x", read_register(CONFIG));
		Serial.println(buf);
		sprintf(buf, "DYNPD/FEATURE\t = 0x%02x 0x%02x", read_register(DYNPD), read_register(FEATURE));
		Serial.println(buf);
		sprintf(buf, "SETUP_RETR\t = 0x%02x", read_register(SETUP_RETR));
		Serial.println(buf);
		Serial.print("Data Rate\t = ");
		switch (read_register(RF_SETUP) & MASK_RF_DR) {
			case RF24_1MBPS:   Serial.println("1 Mbps");   break;
			case RF24_2MBPS:   Serial.println("2 Mbps");   break;
			case RF24_250KBPS: Serial.println("250 kbps"); break;
		}
		sprintf(buf, "CRC Length\t = %d bits", 8 * (bitRead(read_register(CONFIG), CRCO) + 1));
		Serial.println(buf);
		Serial.print("PA Power\t = ");
		switch (read_register(RF_SETUP) & MASK_RF_PWR) {
			case RF24_PA_MIN:  Serial.println("PA_MIN");  break;
			case RF24_PA_LOW:  Serial.println("PA_LOW");  break;
			case RF24_PA_HIGH: Serial.println("PA_HIGH"); break;
			case RF24_PA_MAX:  Serial.println("PA_MAX");  break;
		}
	#endif
	}

	#if !ATtiny13
	void debug() {
		char buf[0x40];
		for (uint8_t i=0; i<0x18; i++)
			sprintf(buf+2*i, "%02x", read_register(i));
		Serial.println(buf);
	}
	#endif

private:
	uint8_t payload_size = max(1, min(32, t_payload_size));
	uint8_t addr_width = 5;

	uint8_t status() {
		return spi(NOP);
	}

	uint8_t read_register(uint8_t addr) {
		uint8_t val;
		spi(R_REGISTER | addr, &val, 1);
		return val;
	}

	uint8_t write_register(uint8_t addr, uint8_t val) {
		return spi(W_REGISTER | addr, &val, 1);
	}

	uint8_t spi(uint8_t cmd, const void *buf = 0, uint8_t len = 0) {
		uint8_t tmp[len];
		if (len)
			memcpy(tmp, buf, len);
		return spi(cmd, tmp, len);
	}

	uint8_t spi(uint8_t cmd, void *buf, uint8_t len) {
		digitalWrite(cs, LOW);
		uint8_t r = SPI.transfer(cmd);
		SPI.transfer(buf, len);
		digitalWrite(cs, HIGH);
		return r;
	}
};

} // namespace rf24

using rf24::RF24;
