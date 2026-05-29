#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>
#include "../Inc/myhal/inc/usart.h"
#define RADIO_SOF1       0xAA
#define RADIO_SOF2       0x55

#define RADIO_MAX_DATA   32

#define DEV_PC          0x00
#define DEV_GREENHOUSE  0x02
#define DEV_BROADCAST   0xFF
#define MY_ID DEV_GREENHOUSE

#define CMD_GREENHOUSE_STATUS 		   0x30
#define CMD_GREENHOUSE_FAN_SET         0x31
#define CMD_GREENHOUSE_PUMP_SET        0x32
#define CMD_GREENHOUSE_AUTOWATER_SET   0x33
#define CMD_GREENHOUSE_AUTOVENT_SET    0x34
#define CMD_GREENHOUSE_SOIL_LIMIT_SET  0x35
#define CMD_STATUS_REQUEST   0x70
#define CMD_STATUS_RESPONSE  0x71

typedef struct {
    uint8_t id;
    uint8_t cmd;
    uint8_t len;
    uint8_t data[RADIO_MAX_DATA];
    uint16_t crc;
} RadioPacket_t;

typedef enum {
    RADIO_RX_WAIT_SOF1,
    RADIO_RX_WAIT_SOF2,
    RADIO_RX_ID,
    RADIO_RX_CMD,
    RADIO_RX_LEN,
    RADIO_RX_DATA,
    RADIO_RX_CRC_LO,
    RADIO_RX_CRC_HI
} RadioRxState_t;

typedef struct {
    RadioRxState_t state;

    RadioPacket_t packet;

    uint8_t data_pos;

    uint8_t crc_buf[3 + RADIO_MAX_DATA];
    uint8_t crc_pos;
} RadioParser_t;
extern RadioParser_t radio_parser;
void RadioParser_Init(RadioParser_t *parser);
bool RadioParser_FeedByte(RadioParser_t *parser, uint8_t byte, RadioPacket_t *out);
void Radio_Init(void);
uint16_t Radio_CRC16(const uint8_t *data, uint16_t len);

void Radio_SendPacket(uint8_t id, uint8_t cmd, const uint8_t *data, uint8_t len);


#endif
