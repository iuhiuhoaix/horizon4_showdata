/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-08-22
 *
 * @copyright Copyright (c) 2023
 *
 */
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

// #define BL618

#define DBG_TAG "MAIN"

struct bflb_device_s *gpio;

u8g2_t __oled;
u8g2_t *oled = &__oled;

TaskHandle_t draw_task_handle;
void draw_task(void *parameter)
{
    u8g2_InitDisplay(oled);
    u8g2_SetPowerSave(oled, 0);
    u8g2_SetContrast(oled, 100);

    int fps = 0;
    char *fps_str;

    uint64_t start_time = 0;
    u8g2_SetFont(oled, u8g2_font_wqy12_t_chinese3);
    while (true)
    {
        start_time = bflb_mtimer_get_time_us();
        fps_str = itoa(fps++, fps_str, 10);
        printf("[str]%s\r\n", fps_str);
        // u8g2_FirstPage(oled);
        // do
        // {
        //     u8g2_DrawUTF8(oled, 0, 15, fps_str);
        // } while (u8g2_NextPage(oled));    

        u8g2_ClearBuffer(oled);

        u8g2_SetFont(oled, u8g2_font_wqy12_t_chinese3);
        {
            u8g2_DrawUTF8(oled, 0, 1 * 16 - 1, fps_str);
        }
        printf("before send\r\n");
        u8g2_SendBuffer(oled);
        printf("after send\r\n");

        // bflb_mtimer_delay_us(33333 - (bflb_mtimer_get_time_us() - start_time));
        // vTaskDelay((33333 - (bflb_mtimer_get_time_us() - start_time)) / (portTICK_PERIOD_MS / 1000));
        vTaskDelay((33333 - (bflb_mtimer_get_time_us() - start_time)) / 1000 / portTICK_PERIOD_MS);
    }
}

TaskHandle_t wifi_task_handle;
void wifi_task(void *parameter)
{
    while (true)
    {
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


    u8g2_Setup_ssd1306_i2c_128x64_noname_f(oled, U8G2_R0, csrc_u8x8_byte_hw_i2c, csrc_u8x8_gpio_and_delay);


    xTaskCreate(draw_task,
                "draw_task",
                256,
                NULL,
                7,
                draw_task_handle);

    xTaskCreate(wifi_task,
                "wifi_task",
                256,
                NULL,
                7,
                wifi_task_handle);

    vTaskStartScheduler();

    // while (1)
    // {
    //     u8g2_ClearBuffer(oled);

    //     u8g2_SetFont(oled, u8g2_font_wqy12_t_chinese3);
    //     for (int i = 1; i <= 4; i++)
    //     {
    //         u8g2_DrawUTF8(oled, 0, i * 16 - 1, "安信可");
    //     }
    //     u8g2_SendBuffer(oled);
    // }

    while (true)
    {
        printf("beacon\r\n");
        bflb_mtimer_delay_ms(10000);
    }

    return 0;
}
