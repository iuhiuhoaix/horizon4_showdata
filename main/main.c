#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include "board.h"
#include "log.h"

#include "bflb_gpio.h"
#include "bflb_irq.h"
#include "bflb_uart.h"
#include "bflb_l1c.h"
#include "bflb_mtimer.h"
#include "bflb_i2c.h"
#include "bflb_uart.h"

#include "easyflash.h"
#include "bflb_flash.h"
#include "bflb_mtd.h"

#include "bl618_u8g2.h"

#include "wifi_event.h"

// #define BL618

#define SSID "鸡你太美1"
#define PASS "jiushijiugejiu"

#define DBG_TAG "MAIN"

struct bflb_device_s *gpio;

uint32_t wifi_status = 0;

TaskHandle_t draw_task_handle;
void draw_task(void *parameter)
{
    u8g2_t __oled;
    u8g2_t *oled = &__oled;

    printf("addr comp: %x -- %x\r\n", oled, &(oled->u8x8));

    // u8g2_Setup_ssd1306_i2c_128x64_noname_f(oled, U8G2_R0, bl618_u8x8_byte_hw_i2c_cb, bl618_u8x8_gpio_and_delay);
    u8g2_Setup_ssd1306_i2c_128x32_univision_f(oled, U8G2_R1, get_u8g2_hw_i2c_cb(oled, 0x78, NULL), get_u8g2_gpio_and_delay(oled));

    printf("stack remain: %d  [start]\r\n", uxTaskGetStackHighWaterMark(draw_task_handle));
    printf("addr of u8g2: %x\r\n", oled);

    u8g2_InitDisplay(oled);
    u8g2_SetPowerSave(oled, 0);
    u8g2_SetContrast(oled, 100);


    int fps = 0;
    char fps_str[10] = {0};
    char wifi_status_str[10] = {0};

    uint64_t start_time = 0;
    u8g2_SetFont(oled, u8g2_font_wqy12_t_chinese3);
    while (bl618_u8x8_get_i2c_error_code(oled) >= 0)
    {

        start_time = bflb_mtimer_get_time_us();
        itoa(fps++, fps_str, 10);
        itoa(*(uint32_t *)parameter, wifi_status_str, 10);
        u8g2_FirstPage(oled);
        do
        {
            u8g2_DrawUTF8(oled, 0, 15, fps_str);
            u8g2_DrawUTF8(oled, 0, 31, wifi_status_str);
        } while (u8g2_NextPage(oled));

        // u8g2_ClearBuffer(oled);

        // u8g2_SetFont(oled, u8g2_font_wqy12_t_chinese3);
        // {
        //     u8g2_DrawUTF8(oled, 0, 1 * 16 - 1, fps_str);
        //     u8g2_DrawUTF8(oled, 0, 2 * 16 - 1, wifi_status_str);
        // }
        // u8g2_SendBuffer(oled);

        // printf("stack remain: %d  [in cycle]\r\n", uxTaskGetStackHighWaterMark(draw_task_handle));
        // bflb_mtimer_delay_us(33333 - (bflb_mtimer_get_time_us() - start_time));
        // vTaskDelay((33333 - (bflb_mtimer_get_time_us() - start_time)) / (portTICK_PERIOD_MS / 1000));
        vTaskDelay((33333 - (bflb_mtimer_get_time_us() - start_time)) / 1000 / portTICK_PERIOD_MS);
    }
}

TaskHandle_t wifi_task_handle;
void wifi_task(void *parameter)
{
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    *(uint32_t *)parameter = wifi_connect(SSID, PASS);
    printf("wifi status: %d\r\n", *(uint32_t *)parameter);

    while (true)
    {
        if (NULL != draw_task_handle)
        {
            eTaskState draw_state = eTaskGetState(draw_task_handle);
            if (draw_state == eBlocked)
            {
                vTaskResume(draw_task_handle);
            }
            printf("task statue: %d\r\n", draw_state);
        }
        printf("beacon\r\n");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void gpio_init()
{
    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_init(gpio, GPIO_PIN_27, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_29, GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_reset(gpio, GPIO_PIN_27);
    bflb_gpio_reset(gpio, GPIO_PIN_29);
}

int main(void)
{
    board_init();
    bflb_mtd_init(); // 这一行不要忘记
    easyflash_init();
    ;
    // uart_init();
    gpio_init();

    xTaskCreate(draw_task,
                "draw_task",
                2048,
                &wifi_status,
                7,
                &draw_task_handle);

    printf("func addr: %x\r\n", wifi_start_firmware_task);

    // wifi_start_firmware_task();

    xTaskCreate(wifi_task,
                "wifi_task",
                2048,
                &wifi_status,
                7,
                wifi_task_handle);

    vTaskStartScheduler();

    printf("end scheduler\r\n");

    while (true)
    {
        bflb_mtimer_delay_ms(10000);
    }

    return 0;
}
