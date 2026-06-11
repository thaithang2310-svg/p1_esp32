#include <stdio.h>
#include "freertos/FreeRTOS.h"

#include "freertos/task.h"
#include "esp_timer.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#define num2 GPIO_NUM_2

// led_state và callback của gptimer:
static volatile uint8_t led_state=0; // chỉ cần xuất ra đúng 2 giá trị đó là 0 và 1
static bool IRAM_ATTR gptimer_callback(gptimer_handle_t handle_timer, const gptimer_alarm_event_data_t *edata, void *user_ctx){
    led_state^=1;
    gpio_set_level(num2,led_state);
    return false; // Không có hàm priority ở đây nên không cần dùng return true -> sử dụng để wake HIGH PRIORITY 
    // return false -> tiếp tục freeRTOS ngay chỗ bị ngắt để thực hiện ISR
}

//app_main: 
void app_main(){
    // Set chân GPIO
    gpio_config_t config_io={
        .pin_bit_mask=(1ULL<<num2),
        .mode=GPIO_MODE_OUTPUT,
        .pull_down_en=GPIO_PULLDOWN_DISABLE,
        .pull_up_en=GPIO_PULLUP_DISABLE,
        .intr_type=GPIO_INTR_DISABLE
    };
    gpio_config(&config_io);

    // Set GPT_timer
    gptimer_handle_t gptimer_handle=NULL;
    gptimer_config_t config_gptimer={
        .clk_src=GPTIMER_CLK_SRC_DEFAULT,
        .direction=GPTIMER_COUNT_UP, // Cấu hình đếm tick từ 0 đi lên 1000000
        .resolution_hz=1000000 // Cấu hình tần số: 1MHz, 1tick=1 chạy tiếp -> đếm lên theo direction 
    };
    gptimer_new_timer(&config_gptimer,&gptimer_handle);

    //Set Callback của hàm timer này
    gptimer_event_callbacks_t gptimer_callback_io={
        .on_alarm=&gptimer_callback,
    };
    gptimer_register_event_callbacks(gptimer_handle, &gptimer_callback_io, NULL);

    //Set alarm-config
    gptimer_alarm_config_t alarm_cf_io={
        .alarm_count=500000, 
        .reload_count=0, 
        .flags.auto_reload_on_alarm=true 
    };
    gptimer_set_alarm_action(gptimer_handle, &alarm_cf_io);

    //Set enable + start
    gptimer_enable(gptimer_handle);
    gptimer_start(gptimer_handle);

    //Hàm lặp thôi ---> EZ 
    while(1){
        vTaskDelay(pdMS_TO_TICKS(100000));
    };
}
