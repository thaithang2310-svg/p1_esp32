#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "freertos/queue.h" // Cách sử dụng cái thằng queue này là gì? ý nghĩa và nguyên lý?
// Define lại hàm cho nó xác định được
// Define này thường được khai báo trong một cái config.h rồi lấy thư viện ra xài thôi 
#define button_pin GPIO_NUM_5
#define led_pin GPIO_NUM_2

// Khai báo các biến của chân led và thời gian
static volatile uint8_t led_state=0;
static volatile uint64_t last=0;
static char *TAG="STATE_BUTTON";

// Hàm ISR để callback lại
static void IRAM_ATTR button_isr_handler(void *arg){
    uint64_t now=esp_timer_get_time();
    int debounce=(now-last)/1000;
    if (debounce>=50){
        last=now;
        led_state^=1;
        gpio_set_level(led_pin,led_state);
        // ESP_LOGI(TAG,"LED STATE: %d",led_state?"ON":"OFF"); -> Luôn nhớ không được gọi ESP_LOGI trong ISR 
    }
}

void app_main(void)
{
    //set chân GPIO LED
    gpio_config_t led_pin_io={
        .pin_bit_mask=(1ULL<<led_pin),
        .mode=GPIO_MODE_OUTPUT,
        .pull_down_en=GPIO_PULLDOWN_DISABLE,
        .pull_up_en=GPIO_PULLUP_DISABLE,
        .intr_type=GPIO_INTR_DISABLE,
    };
    gpio_config(&led_pin_io);
    
    //Set chân GPIO Button
    gpio_config_t button_pin_io={
        .pin_bit_mask=(1ULL<<button_pin),
        .mode=GPIO_MODE_INPUT,
        .pull_down_en=GPIO_PULLDOWN_ENABLE,
        .pull_up_en=GPIO_PULLUP_DISABLE,
        .intr_type=GPIO_INTR_POSEDGE,
    };
    gpio_config(&button_pin_io);

    //Thông báo được dùng chân GPIO qua hàm ngắt (ISR)
    gpio_install_isr_service(0);
    //Đơn đăng kí xin ISR
    gpio_isr_handler_add(button_pin,button_isr_handler,NULL);
    //Action
    ESP_LOGI(TAG,"Nút nhấn đã sẵn sàng!");
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(10000));  
    };
}
