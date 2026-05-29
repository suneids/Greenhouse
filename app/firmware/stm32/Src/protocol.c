#include "protocol.h"

#define MY_DEVICE_ID 0x02

RadioParser_t radio_parser;
uint16_t Radio_CRC16(const uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;

    for(uint16_t i = 0; i < len; i++) {
        crc ^= data[i];

        for(uint8_t bit = 0; bit < 8; bit++) {
            if(crc & 1) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}

void RadioParser_Init(RadioParser_t *parser)
{
    parser->state = RADIO_RX_WAIT_SOF1;
    parser->data_pos = 0;
    parser->crc_pos = 0;
}

bool RadioParser_FeedByte(RadioParser_t *parser, uint8_t byte, RadioPacket_t *out)
{
    switch(parser->state) {
        case RADIO_RX_WAIT_SOF1:
            if(byte == RADIO_SOF1) {
                parser->state = RADIO_RX_WAIT_SOF2;
            }
            break;

        case RADIO_RX_WAIT_SOF2:
            if(byte == RADIO_SOF2) {
                parser->state = RADIO_RX_ID;
            } else if(byte == RADIO_SOF1) {
                parser->state = RADIO_RX_WAIT_SOF2;
            } else {
                parser->state = RADIO_RX_WAIT_SOF1;
            }
            break;

        case RADIO_RX_ID:
            parser->packet.id = byte;
            parser->crc_buf[0] = byte;
            parser->state = RADIO_RX_CMD;
            break;

        case RADIO_RX_CMD:
            parser->packet.cmd = byte;
            parser->crc_buf[1] = byte;
            parser->state = RADIO_RX_LEN;
            break;

        case RADIO_RX_LEN:
            parser->packet.len = byte;
            parser->crc_buf[2] = byte;

            if(parser->packet.len > RADIO_MAX_DATA) {
                parser->state = RADIO_RX_WAIT_SOF1;
            } else if(parser->packet.len == 0) {
                parser->state = RADIO_RX_CRC_LO;
            } else {
                parser->data_pos = 0;
                parser->state = RADIO_RX_DATA;
            }
            break;

        case RADIO_RX_DATA:
            parser->packet.data[parser->data_pos] = byte;
            parser->crc_buf[3 + parser->data_pos] = byte;

            parser->data_pos++;

            if(parser->data_pos >= parser->packet.len) {
                parser->state = RADIO_RX_CRC_LO;
            }
            break;

        case RADIO_RX_CRC_LO:
            parser->packet.crc = byte;
            parser->state = RADIO_RX_CRC_HI;
            break;

        case RADIO_RX_CRC_HI: {
            parser->packet.crc |= ((uint16_t)byte << 8);

            uint16_t calc = Radio_CRC16(
                parser->crc_buf,
                3 + parser->packet.len
            );

            parser->state = RADIO_RX_WAIT_SOF1;

            if(calc == parser->packet.crc) {
                *out = parser->packet;
                return true;
            }

            break;
        }

        default:
            parser->state = RADIO_RX_WAIT_SOF1;
            break;
    }

    return false;
}


void Radio_Init(void)
{
    RadioParser_Init(&radio_parser);
}


void Radio_SendPacket(uint8_t id, uint8_t cmd, const uint8_t *data, uint8_t len)
{
    uint8_t buf[64];
    uint8_t p = 0;

    buf[p++] = 0xAA;
    buf[p++] = 0x55;

    buf[p++] = id;
    buf[p++] = cmd;
    buf[p++] = len;

    for(uint8_t i = 0; i < len; i++) {
        buf[p++] = data[i];
    }

    uint16_t crc = Radio_CRC16(&buf[2], 3 + len);

    buf[p++] = crc & 0xFF;
    buf[p++] = (crc >> 8) & 0xFF;

    for(uint8_t i = 0; i < p; i++) {
        USART_WriteByte(USART1, buf[i]); // или USART2, какой у тебя на G0
    }
}



