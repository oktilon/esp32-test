/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO

static uint8_t s_led_state = 0;

#ifdef CONFIG_BLINK_LED_RMT
const uint8_t palette[22][3] = {
    {128, 0  , 0  }, // Red
    {128, 32 , 0  },
    {128, 64 , 0  }, // Orange
    {128, 96 , 0  },
    {128, 128, 0  }, // Yellow
    {96 , 128, 0  },
    {64 , 128, 0  },
    {32 , 128, 0  },
    {0  , 128, 0  }, // Green
    {0  , 128, 32 },
    {0  , 128, 64 },
    {0  , 128, 96 },
    {0  , 128, 128}, // Cyan
    {0  , 96 , 128},
    {0  , 64 , 128},
    {0  , 32 , 128},
    {0  , 0  , 128}, // Blue
    {32 , 0  , 128},
    {64 , 0  , 128}, // Violet
    {64 , 0  , 96 },
    {96 , 0  , 64 },
    {96 , 0  , 32 }
};
static led_strip_t *pStrip_a;
const uint8_t led_count = 16;
const uint8_t tail_len = 8;
uint8_t led_color = 0;
uint8_t led_tail = 0;
uint8_t aR[8] = {0};
uint8_t aG[8] = {0};
uint8_t aB[8] = {0};

static void blink_led(void)
{
    uint8_t ix = 0, led_ix = 0;
    uint8_t r = 0, g = 0, b = 0;
    pStrip_a->clear(pStrip_a, 50);
    for(ix = 0; ix < (tail_len + 1); ix++) {
        led_ix = (led_tail + ix) % led_count;
        if(ix == 0) {
            pStrip_a->set_pixel(pStrip_a, led_ix, 0,  0, 0);
        } else {
            if(ix < tail_len) {
                r = aR[ix] / (tail_len - ix);
                g = aG[ix] / (tail_len - ix);
                b = aB[ix] / (tail_len - ix);
                aR[ix - 1] = aR[ix];
                aG[ix - 1] = aG[ix];
                aB[ix - 1] = aB[ix];
            } else {
                led_color++;
                if(led_color > 21) led_color = 0;
                aR[ix - 1] = palette[led_color][0];
                aG[ix - 1] = palette[led_color][1];
                aB[ix - 1] = palette[led_color][2];
                r = aR[ix - 1];
                g = aG[ix - 1];
                b = aB[ix - 1];
            }
            pStrip_a->set_pixel(pStrip_a, led_ix, r,  g, b);
            //ESP_LOGI(TAG, "Ix=%d [%d,%d,%d]", led_ix, r, g, b);
        }
    }
    ESP_LOGI(TAG, "Tail=%d, HeadIx=%d [%d,%d,%d]", led_tail, led_ix, r, g, b);
    led_tail++;
    if(led_tail >= led_count) led_tail = 0;
    pStrip_a->refresh(pStrip_a, 100);
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    pStrip_a = led_strip_init(CONFIG_BLINK_LED_RMT_CHANNEL, BLINK_GPIO, led_count);
    /* Set all LED off to clear all pixels */
    pStrip_a->clear(pStrip_a, 50);
}

#elif CONFIG_BLINK_LED_GPIO

static void blink_led(void)
{
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(BLINK_GPIO, s_led_state);
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

#endif

void app_main(void)
{

    /* Configure the peripheral according to the LED type */
    configure_led();

    while (1) {
        // ESP_LOGI(TAG, "Turning the LED [pin:%d] %s!", BLINK_GPIO, s_led_state == true ? "ON" : "OFF");
        blink_led();
        /* Toggle the LED state */
        s_led_state = !s_led_state;
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
