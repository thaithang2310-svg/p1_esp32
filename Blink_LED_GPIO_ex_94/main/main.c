/*
Bài 4: Blink nhiều LED với chu kỳ khác nhau

Yêu cầu:
- LED 1 (GPIO 2): nhấp nháy 1Hz
- LED 2 (GPIO 4): nhấp nháy 3Hz
- LED 3 (GPIO 5): nhấp nháy 5Hz

Yêu cầu: tạo 3 timer riêng biệt, mỗi timer điều khiển một LED.

Gợi ý: truyền số chân GPIO vào tham số arg của callback thay vì dùng biến global.

```c
// Cách truyền arg vào callback
static void blink_callback(void *arg)
{
    int gpio_num = (int)arg;  // ép kiểu void* về int
    // toggle GPIO gpio_num
}

// Khi tạo timer:
const esp_timer_create_args_t timer_args = {
    .callback = &blink_callback,
    .arg      = (void *)GPIO_NUM_2,  // truyền số chân vào
    .name     = "led1_timer",
};
```
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "driver/uart.h"

#include "esp_timer.h"
#include "esp_log.h"

static char *TAG="LED STATE";

void esp_timer_callback(void *arg){
    int gpio_num_io=(int)arg;
    static int state=0;
    state=!state;
    gpio_set_level(gpio_num_io,state);
    ESP_LOGI(TAG,"Trang thai led %d: %s",gpio_num_io,state?"ON":"OFF");
}

void app_main(void)
{
    // set chân gpio
    gpio_config_t config_io={
        .pin_bit_mask=(1ULL<<GPIO_NUM_2)|(1ULL<<GPIO_NUM_4)|(1ULL<<GPIO_NUM_5),
        .mode=GPIO_MODE_OUTPUT,
        .intr_type=GPIO_INTR_DISABLE,
        .pull_down_en=GPIO_PULLDOWN_DISABLE,
        .pull_up_en=GPIO_PULLUP_DISABLE,
    };
    gpio_config(&config_io);
    // khai báo esp_timer
    //Chân 2
    esp_timer_handle_t timer_handle2=NULL;
    esp_timer_create_args_t esp_timer_cb2={
        .callback=&esp_timer_callback,
        .arg=(void *)GPIO_NUM_2,
        .name="Chân thứ 2"
    };
    esp_timer_create(&esp_timer_cb2,&timer_handle2);
    //Chân 4
    esp_timer_handle_t timer_handle4=NULL;
    esp_timer_create_args_t esp_timer_cb4={
        .callback=&esp_timer_callback,
        .arg=(void *)GPIO_NUM_4,
        .name="Chân thứ 4"
    };
    esp_timer_create(&esp_timer_cb4,&timer_handle4);
    //Chân 5
    esp_timer_handle_t timer_handle5=NULL;
    esp_timer_create_args_t esp_timer_cb5={
        .callback=&esp_timer_callback,
        .arg=(void *)GPIO_NUM_5,
        .name="Chân thứ 5"
    };
    esp_timer_create(&esp_timer_cb5,&timer_handle5);
    // action thôi 
    esp_timer_start_periodic(timer_handle2,500000);
    esp_timer_start_periodic(timer_handle4,166666);
    esp_timer_start_periodic(timer_handle5,100000);
    // loop while(1)
    while(1){
        vTaskDelay(pdMS_TO_TICKS(1000000));
    }
}
