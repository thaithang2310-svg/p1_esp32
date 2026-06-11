#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

static volatile uint8_t led_state=0; //Kiểu dữ liệu gì
static const char *TAG="LED_STATE";
//Callback
void esp_timer_callback(void *arg){
    led_state^=1;
    gpio_set_level(GPIO_NUM_2, led_state);
    ESP_LOGI(TAG,"Trạng thái: %s", led_state?"ON":"OFF"); // Nhớ hàm cần cấp ESP_LOGX
}

void app_main(void)
{
    // Set GPIO 
    gpio_config_t gpio_io={
        .pin_bit_mask=(1ULL<<GPIO_NUM_2),
        .mode=GPIO_MODE_OUTPUT,
        .pull_down_en=GPIO_PULLDOWN_DISABLE,
        .pull_up_en=GPIO_PULLUP_DISABLE, // Nhớ pullup pulldown đi theo 1 cặp
        .intr_type=GPIO_INTR_DISABLE,
    };
    gpio_config(&gpio_io);

    // Set Callback + timer_handle
    esp_timer_create_args_t esp_timer_cb_io={
        .callback=&esp_timer_callback,
        .arg=NULL,
        .name="GPIO2"
    };
    esp_timer_handle_t timer_handle=NULL;
    // Link cái callback với timer_handle
    esp_timer_create(&esp_timer_cb_io,&timer_handle);
    esp_timer_start_periodic(timer_handle, 250000);
    while(1){
        vTaskDelay(pdMS_TO_TICKS(10000));
    };
}
