#include "oled.h"

void i2c_transfer_(uint8_t addr, size_t len, uint8_t *buffer)
{
    struct bflb_i2c_msg_s msgs;

    msgs.addr = addr;
    msgs.flags = 0;
    msgs.buffer = buffer;
    msgs.length = len;

    printf("len: %d \r\n", len);    

    bflb_i2c_transfer(__i2c, &msgs, 1);
}


uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{

    static uint8_t buffer[256]; /* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
    static uint8_t buf_idx;
    uint8_t *data;

    switch (msg)
    {
    case U8X8_MSG_BYTE_SEND:
        data = (uint8_t *)arg_ptr;
        while (arg_int > 0)
        {
            buffer[buf_idx++] = *data;
            data++;
            arg_int--;
        }
        break;
    case U8X8_MSG_BYTE_INIT:
        __gpio = bflb_device_get_by_name("gpio");

        bflb_gpio_init(__gpio, GPIO_PIN_0, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(__gpio, GPIO_PIN_1, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);

        __i2c = bflb_device_get_by_name("i2c0");
        bflb_i2c_init(__i2c, 400000);
        break;
    case U8X8_MSG_BYTE_SET_DC:
        /* ignored for i2c */
        break;
    case U8X8_MSG_BYTE_START_TRANSFER:
        buf_idx = 0;
        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        i2c_transfer_(u8x8_GetI2CAddress(u8x8) >> 1, buf_idx, buffer);

        break;
    default:
        return 0;
    }
    return 1;
}


uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_GPIO_AND_DELAY_INIT: // called once during init phase of u8g2/u8x8
        break;                         // can be used to setup pins
    case U8X8_MSG_DELAY_NANO:          // delay arg_int * 1 nano second
        break;
    case U8X8_MSG_DELAY_100NANO:
        for (int i = 0;i < 10;++i);
        break;
    case U8X8_MSG_DELAY_10MICRO:
        bflb_mtimer_delay_us(10 * arg_int);
        break;
    case U8X8_MSG_DELAY_MILLI:
        bflb_mtimer_delay_ms(arg_int);
        break;
    case U8X8_MSG_DELAY_I2C: // arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
        break;               // arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
    case U8X8_MSG_GPIO_D0:   // D0 or SPI clock pin: Output level in arg_int
                             // case U8X8_MSG_GPIO_SPI_CLOCK:
        break;
    case U8X8_MSG_GPIO_D1: // D1 or SPI data pin: Output level in arg_int
                           // case U8X8_MSG_GPIO_SPI_DATA:
        break;
    case U8X8_MSG_GPIO_D2: // D2 pin: Output level in arg_int
        break;
    case U8X8_MSG_GPIO_D3: // D3 pin: Output level in arg_int
        break;
    case U8X8_MSG_GPIO_D4: // D4 pin: Output level in arg_int
        break;
    case U8X8_MSG_GPIO_D5: // D5 pin: Output level in arg_int
        break;
    case U8X8_MSG_GPIO_D6: // D6 pin: Output level in arg_int
        break;
    case U8X8_MSG_GPIO_D7: // D7 pin: Output level in arg_int
        break;
    case U8X8_MSG_GPIO_E: // E/WR pin: Output level in arg_int
        break;
    case U8X8_MSG_GPIO_CS: // CS (chip select) pin: Output level in arg_int
        break;
    case U8X8_MSG_GPIO_DC: // DC (data/cmd, A0, register select) pin: Output level in arg_int
        break;
    case U8X8_MSG_GPIO_RESET: // Reset pin: Output level in arg_int
        break;
    case U8X8_MSG_GPIO_CS1: // CS1 (chip select) pin: Output level in arg_int
        break;
    case U8X8_MSG_GPIO_CS2: // CS2 (chip select) pin: Output level in arg_int
        break;
    case U8X8_MSG_GPIO_I2C_CLOCK: // arg_int=0: Output low at I2C clock pin
        break;                    // arg_int=1: Input dir with pullup high for I2C clock pin
    case U8X8_MSG_GPIO_I2C_DATA:  // arg_int=0: Output low at I2C data pin
        break;                    // arg_int=1: Input dir with pullup high for I2C data pin
    case U8X8_MSG_GPIO_MENU_SELECT:
        u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_NEXT:
        u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_PREV:
        u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
        break;
    case U8X8_MSG_GPIO_MENU_HOME:
        u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
        break;
    default:
        u8x8_SetGPIOResult(u8x8, 1); // default return value
        break;
    }
    return 1;
}
