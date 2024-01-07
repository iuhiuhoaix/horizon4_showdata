#include "bl618_u8g2.h"

struct screen* head = NULL;

// 获取当前u8g2句柄
struct screen* get_u8g2_instance(u8x8_t* u8x8)
{
    struct screen* res = head;
    while (res && (res->__u8g2 != u8x8))
    {
        // if (res->__u8x8 == NULL)
        // {

        // }
        res = res->next;
    }
    return res;
}

// void i2c_trans(uint8_t addr, uint8_t* buf, uint8_t len)
// {
//     // struct bflb_i2c_msg_s msg = {
//     //     .addr = 0x3c,
//     //     .__buffer = buf,
//     //     .flags = 0,
//     //     .length = len
//     // };
//     struct bflb_i2c_msg_s msg;
//     msg.addr = 0x3c;
//     msg.__buffer = buf;
//     msg.length = len;
//     msg.flags = 0;

//     if (bflb_i2c_transfer(instance->__u8g2_i2c, &msg, 1) != 0)
//     {

//         printf("reset i2c\r\n");
//         // bflb_i2c_int_clear(__u8g2_i2c, bflb_i2c_get_intstatus(__u8g2_i2c));
//     }
// }

int u8g2_i2c_init(struct screen* instance)
{


    struct bflb_device_s *gpio;
    gpio = bflb_device_get_by_name("gpio");
    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
    instance->__u8g2_i2c = bflb_device_get_by_name("i2c0");
    bflb_i2c_init(instance->__u8g2_i2c, 400000);
    bflb_i2c_link_txdma(instance->__u8g2_i2c, true);
    instance->__u8g2_i2c_msg.length = 0;
    // __u8g2_i2c_msg.flags = I2C_M_DMA;
    instance->__u8g2_i2c_msg.flags = 0;
    printf("[oninit]buffer addr: %x\r\n", instance->__u8g2_i2c_msg.buffer);

    return 0;
}

uint8_t bl618_u8x8_byte_hw_i2c_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{   
    struct screen* instance = get_u8g2_instance(u8x8);

    uint8_t *data;
    switch (msg)
    {
    case U8X8_MSG_BYTE_INIT: 
        if (instance->__u8g2_i2c == NULL)
        {
            u8g2_i2c_init(instance);
        }
        break;
    case U8X8_MSG_BYTE_START_TRANSFER:
        instance->__idx = 0;
        break;
    case U8X8_MSG_BYTE_SEND:
        data = (uint8_t *)arg_ptr;
        while (arg_int > 0)
        {
            instance->__buf[instance->__idx++] = *data;
            data++;
            --arg_int;
        }
        break;
    case U8X8_MSG_BYTE_END_TRANSFER:
        instance->__u8g2_i2c_msg.length = instance->__idx;
        if ((bflb_i2c_transfer(instance->__u8g2_i2c, &instance->__u8g2_i2c_msg, 1)) != 0)
        {
            printf("u8g2 transmit error code: %d\r\n", instance->error_code);
            // csrc_u8x8_byte_hw_i2c(u8x8, U8X8_MSG_BYTE_INIT, 0, NULL);
            u8g2_i2c_init(instance); 
        };
        break;
    case U8X8_MSG_BYTE_SET_DC:

        break;
    default:
        return 0;
    }
    return 1;
}



uint8_t bl618_u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
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

u8x8_msg_cb get_u8g2_hw_i2c_cb(u8x8_t *u8g2, uint8_t i2c_addr, struct bflb_device_s *i2c)
{
    if (head == NULL)
    {
        head = (struct screen*)malloc(sizeof(struct screen));
        head->__u8g2_i2c = i2c;
        head->next = NULL;
    }

    struct screen *instance = head;

    while (instance->next)
    {
        instance = instance->next;
    }
    
    instance->__u8g2 = u8g2;

    if (i2c != NULL)
    {
        instance->__u8g2_i2c = i2c;
    }
    else
    {
        instance->__u8g2_i2c = head->__u8g2_i2c;
    }

    instance->next = NULL;


    if (instance->__u8g2_i2c == NULL)
    {
        struct bflb_device_s *gpio;
        gpio = bflb_device_get_by_name("gpio");
        bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_FUNC_I2C0 | GPIO_ALTERNATE | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_1);
        instance->__u8g2_i2c = bflb_device_get_by_name("i2c0");
        bflb_i2c_init(instance->__u8g2_i2c, 400000);
        bflb_i2c_link_txdma(instance->__u8g2_i2c, true);
    }

    instance->__u8g2_i2c_msg.addr = i2c_addr >> 1;
    instance->__u8g2_i2c_msg.flags = 0;
    instance->__u8g2_i2c_msg.length = 0;
    instance->__u8g2_i2c_msg.buffer = instance->__buf = (uint8_t*)malloc(U8G2_I2C_BUF_SIZE);

    return bl618_u8x8_byte_hw_i2c_cb;
}

uint32_t bl618_u8x8_get_i2c_error_code(u8x8_t *u8x8)
{
    return 0;
}

u8x8_msg_cb get_u8g2_gpio_and_delay(u8x8_t *u8x8)
{
    return bl618_u8x8_gpio_and_delay;
}