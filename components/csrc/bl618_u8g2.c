#include "bl618_u8g2.h"

void i2c_trans(uint8_t addr, uint8_t* buf, uint8_t len)
{
    // struct bflb_i2c_msg_s msg = {
    //     .addr = 0x3c,
    //     .buffer = buf,
    //     .flags = 0,
    //     .length = len
    // };
    _msg.addr = 0x3c;
    _msg.buffer = buf;
    _msg.length = len;
    _msg.flags = 0;

    if (bflb_i2c_transfer(__u8g2_i2c, &_msg, 1) != 0)
    {

        printf("reset i2c\r\n");
        // bflb_i2c_int_clear(__u8g2_i2c, bflb_i2c_get_intstatus(__u8g2_i2c));
    }
}

uint8_t csrc_u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{   
    static uint8_t buf[256];
    static int32_t idx = 0;
    uint8_t *data;

    static struct bflb_device_s *gpio;

    switch (msg)
    {
    case U8X8_MSG_BYTE_INIT: 
        gpio = bflb_device_get_by_name("gpio");
        bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        __u8g2_i2c = bflb_device_get_by_name("i2c0");
        bflb_i2c_init(__u8g2_i2c, 400000);
        bflb_i2c_link_txdma(__u8g2_i2c, true);
        __u8g2_i2c_msg.addr = OLED_ADDRESS >> 1;
        __u8g2_i2c_msg.buffer = buf;
        __u8g2_i2c_msg.length = 0;
        // __u8g2_i2c_msg.flags = I2C_M_DMA;
        __u8g2_i2c_msg.flags = 0;
        printf("[oninit]buffer addr: %x\r\n", __u8g2_i2c_msg.buffer);
        break;
    case U8X8_MSG_BYTE_START_TRANSFER:
        idx = 0;
        break;
    case U8X8_MSG_BYTE_SEND:
        data = (uint8_t *)arg_ptr;
        while (arg_int > 0)
        {
            buf[idx++] = *data;
            data++;
            --arg_int;
        }
        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        __u8g2_i2c_msg.length = idx;
        int res = 0;
        if ((res = bflb_i2c_transfer(__u8g2_i2c, &__u8g2_i2c_msg, 1)) != 0)
        {
            printf("error code: %d", res);
            // csrc_u8x8_byte_hw_i2c(u8x8, U8X8_MSG_BYTE_INIT, 0, NULL);
            bflb_i2c_deinit(__u8g2_i2c);
            idx = 0;
        };
        // bflb_mtimer_delay_ms(10);
        // i2c_trans(0x3c, buf, idx);
        break;
    case U8X8_MSG_BYTE_SET_DC:

        break;
    default:
        return 0;
    }
    return 1;
}



uint8_t csrc_u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_DELAY_100NANO:
        for (int i = 0;i < 10;++i);
        break;
    case U8X8_MSG_DELAY_10MICRO:
        bflb_mtimer_delay_us(10 * arg_int);
        break;
    case U8X8_MSG_DELAY_MILLI:
        bflb_mtimer_delay_ms(arg_int);
        break;
    case U8X8_MSG_GPIO_MENU_SELECT:
        break;
    case U8X8_MSG_GPIO_MENU_NEXT:
        break;
    case U8X8_MSG_GPIO_MENU_PREV:
        break;
    case U8X8_MSG_GPIO_MENU_HOME:
        break;
    default:
        break;
    }
    return 1;
}

// void u8g2Init(u8g2_t *u8g2);
// void draw(u8g2_t *u8g2);
// void testDrawPixelToFillScreen(u8g2_t *u8g2);
