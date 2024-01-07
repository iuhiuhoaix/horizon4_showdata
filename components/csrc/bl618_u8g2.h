#ifndef __BL618_U8G2_H
#define __BL618_U8G2_H

#include "u8g2.h"

#include "bl_fw_api.h"
#include "bflb_i2c.h"
#include "bflb_gpio.h"
#include "bflb_mtimer.h"
#include "bflb_dma.h"
#include "board.h"
#include "bflb_core.h"


#define u8                  unsigned char
#define U8G2_I2C_BUF_SIZE   256

typedef struct screen{
    struct bflb_device_s *__u8g2_i2c;
    struct bflb_i2c_msg_s __u8g2_i2c_msg;
    uint32_t __idx;
    int32_t error_code;
    uint8_t *__buf;
    u8g2_t *__u8g2;
    struct screen* next;
};

u8x8_msg_cb get_u8g2_hw_i2c_cb(u8x8_t *u8x8, uint8_t __i2c_addr, struct bflb_device_s *i2c);
u8x8_msg_cb get_u8g2_gpio_and_delay(u8x8_t *u8x8);
// void i2c_trans(uint8_t addr, uint8_t* buf, uint8_t len);
// uint8_t bl618_u8x8_byte_hw_i2c_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
// uint8_t bl618_u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint32_t bl618_u8x8_get_i2c_error_code(u8x8_t *u8x8);
// void u8g2Init(u8g2_t *u8g2);
// void draw(u8g2_t *u8g2);
// void testDrawPixelToFillScreen(u8g2_t *u8g2);

#endif