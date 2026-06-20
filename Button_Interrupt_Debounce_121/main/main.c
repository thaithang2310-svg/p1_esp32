#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define button_pin  GPIO_NUM_5
#define gpio_pin    GPIO_NUM_15
#define debounce_MS 50

static volatile uint8_t  led_state  = 0;
static volatile uint64_t now        = 0;
static volatile uint8_t  print_flag = 0;
static const char *TAG = "LED_STATE";

void IRAM_ATTR isr_callback_button(void *arg) {
    uint32_t gpio_cast = (uint32_t)arg;
    uint64_t last      = esp_timer_get_time();
    uint64_t debounce  = (last - now) / 1000;

    if (debounce >= debounce_MS) {
        now = last;
        led_state ^= 1;
        gpio_set_level(gpio_cast, led_state);
        print_flag = 1;
    }
}

void app_main(void) {
    // Cấu hình GPIO15 (LED chính)
    gpio_config_t gpiopin_io = {
        .mode         = GPIO_MODE_OUTPUT,
        .intr_type    = GPIO_INTR_DISABLE,
        .pin_bit_mask = (1ULL << gpio_pin),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&gpiopin_io);

    gpio_set_level(GPIO_NUM_2, 1);  // Bật cứng GPIO2

    // Cấu hình Button
    gpio_config_t buttonpin_io = {
        .mode         = GPIO_MODE_INPUT,
        .intr_type    = GPIO_INTR_NEGEDGE,
        .pin_bit_mask = (1ULL << button_pin),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&buttonpin_io);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(button_pin, isr_callback_button, (void *)button_pin);

    // Vòng lặp chính
    while (1) {
        if (print_flag) {
            print_flag = 0;
            ESP_LOGI(TAG, "Button pressed! LED: %s", led_state ? "ON" : "OFF");
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}