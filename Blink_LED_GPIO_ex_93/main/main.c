/*
Bài 3: Đếm số lần LED đã blink
Yêu cầu: in ra UART tổng số lần LED đã toggle sau mỗi 10 lần.
Gợi ý: thêm biến đếm trong callback, dùng modulo (%) để in mỗi 10 lần.
Nên khi đề bài nói "in ra UART" và bạn thấy chữ hiện trên monitor — đó là một việc, không phải hai 
việc khác nhau. ESP_LOGI đã ghi vào UART0 rồi, monitor chỉ là tool đọc cái đó lên màn hình.
ESP_LOGI() → UART0 TX pin → USB-Serial chip (CP210x) → USB → máy tính → idf.py monitor hiển thị
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "driver/uart.h"

static volatile uint8_t led_state=0; //Kiểu dữ liệu gì
static const char *TAG="LED_STATE";
static uint64_t toggle_count=0;
//Callback
void esp_timer_callback(void *arg){

    led_state^=1;
    toggle_count+=1;
    gpio_set_level(GPIO_NUM_2, led_state);
    if (toggle_count%10==0){
        ESP_LOGI(TAG,"Tổng số lần toggle: %llu",toggle_count);
    }
}

void app_main(void)
{
    // Set UART
    uart_config_t uart_io={
        .baud_rate=115200,
        .data_bits=UART_DATA_8_BITS,
        .parity=UART_PARITY_DISABLE,
        .stop_bits=UART_STOP_BITS_1,
        .flow_ctrl=UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0,&uart_io);
    uart_driver_install(UART_NUM_0,1024,0,0,NULL,0);

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
    esp_timer_start_periodic(timer_handle, 100000);
    /*Cần phải set sẵn mạch sẽ chạy với tốc độ bao nhiêu Hz đó để không bị treo chờ Serial*/
    while(1){
        vTaskDelay(pdMS_TO_TICKS(1000000));
    };
}
