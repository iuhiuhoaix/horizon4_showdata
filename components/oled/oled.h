#ifndef __oled_H
#define __oled_H

#include "bflb_gpio.h"
#include "bflb_i2c.h"

#include "u8g2.h"

static struct bflb_device_s *__gpio;
static struct bflb_device_s *__i2c;


void i2c_transfer_(uint8_t addr, size_t len, uint8_t *buffer);

uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

#endif