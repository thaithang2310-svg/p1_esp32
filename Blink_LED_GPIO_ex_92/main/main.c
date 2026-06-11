#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "driver/uart.h"

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
    esp_timer_start_periodic(timer_handle, 250000);
    /*Cần phải set sẵn mạch sẽ chạy với tốc độ bao nhiêu Hz đó để không bị treo chờ Serial*/
    
    while(1){
        uint8_t rx_buf[1]={0};
        int len=uart_read_bytes(UART_NUM_0,rx_buf,1,pdMS_TO_TICKS(100));
        if (len>0){ // Nếu len nhận được giá trị
            uint64_t period_us=0;
            switch(rx_buf[0]){
                case '1': period_us=500000; break;
                case '2': period_us=250000; break;
                case '5': period_us=100000; break;
            };
            if (period_us>0){
                esp_timer_stop(timer_handle);
                esp_timer_start_periodic(timer_handle, period_us);
                ESP_LOGI(TAG,"Nhan %c, period: %llu us", rx_buf[0], period_us);
            }
        }
    };
}
