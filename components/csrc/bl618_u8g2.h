#ifndef __BL618_U8G2_H
#define __BL618_U8G2_H

#include "u8g2.h"

#include "bl_fw_api.h"
#include "bflb_i2c.h"
#include "bflb_gpio.h"
#include "bl616_gpio.h"
#include "bflb_mtimer.h"
#include "bflb_dma.h"
#include "board.h"

#define u8              unsigned char
#define MAX_LEN         128
#define OLED_ADDRESS    0x78
#define OLED_CMD        0x00
#define OLED_DATA       0x40

static struct bflb_device_s *__u8g2_i2c;
static struct bflb_i2c_msg_s __u8g2_i2c_msg;
static struct bflb_i2c_msg_s _msg;

void i2c_trans(uint8_t addr, uint8_t* buf, uint8_t len);
uint8_t csrc_u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t csrc_u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
// void u8g2Init(u8g2_t *u8g2);
// void draw(u8g2_t *u8g2);
// void testDrawPixelToFillScreen(u8g2_t *u8g2);

#endif