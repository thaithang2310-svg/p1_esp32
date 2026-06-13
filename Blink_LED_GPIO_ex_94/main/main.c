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
#include "esp_timer.h"
#include "esp_log.h"

static volatile uint8_t led_state=0;
static char *TAG="LED STATE: ";
void esp_timer_callback(void *arg){
    led_state^=1;
    int gpio_num_io=(int)arg;
    ESP_LOGI(TAG,"Trang thai led: %s",led_state?"ON":"OFF");
}

void app_main(void)
{

}
