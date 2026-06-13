/*
Yêu cầu: không dùng callback, thay vào đó trong vòng lặp while(1) dùng esp_timer_get_time() để đọc thời gian 
hiện tại và tự quyết định khi nào toggle LED.

```c
uint64_t last_toggle = 0;

while (1) {
    uint64_t now = esp_timer_get_time();  // trả về microsecond từ khi boot
    if (now - last_toggle >= 500000) {    // 500ms đã qua
        led_state ^= 1;
        gpio_set_level(LED_GPIO, led_state);
        last_toggle = now;
    }
    // CPU không bị block, có thể làm việc khác ở đây
}
```

Mục đích bài này: hiểu rõ sự khác biệt giữa cách tiếp cận event-driven (callback) và polling. Cả hai đều không dùng delay().
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

// Không có callback -> chỉ có led_state,last toggle và tag để hiển thị monitor
static volatile uint8_t led_state=0;
static char *TAG="GPIO_LED2";
uint64_t last_toggle=0;

void app_main(void)
{
    // Set chân GPIO 
    gpio_config_t gpio_io={
        .pin_bit_mask=GPIO_NUM_2,
        .mode=GPIO_MODE_OUTPUT,
        .pull_down_en=GPIO_PULLDOWN_DISABLE,
        .pull_up_en=GPIO_PULLUP_DISABLE,
        .intr_type=GPIO_INTR_DISABLE,
    };
    gpio_config(&gpio_io);
    uint64_t now=esp_timer_get_time();
    // Action: không cần callback ở đây
    while(1){
        if (now-last_toggle>=250000){
            led_state^=1;
            gpio_set_level(GPIO_NUM_2,led_state);
            ESP_LOGI(TAG,"Gia tri: %d tai thoi diem: %d",led_state?"ON":"OFF",now);
            last_toggle=now;
        };
    };
}
