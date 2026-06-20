// nhấn nút để bật/tắt timer blink LED — đây là bài tập mở rộng tự nhiên.(example extra) ->debugging bằng uart -> luyện tư duy debug ==
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_log.h" 
#include "driver/gpio.h"
#include "driver/uart.h"
// define: debounce time, gpio_pin, button_pin
#define BUTTON_PIN GPIO_NUM_5
#define GPIO_PIN GPIO_NUM_2
#define DEBOUNCE_MS 50 // Ở đây là 50ms 

// Các tham số bị hardware điều khiển: 
static const char *TAG="LED_STATE";
static volatile uint32_t button_count=0;
static volatile uint8_t led_state=0;
static volatile uint64_t now=0;

// Timer handle buộc phải scope global để hàm callback ISR đọc được 
esp_timer_handle_t timer_handle=NULL;

// callback blink led (GPIO phải được truyền từ arg chứ không phải void trống)
void gpio_cb_esp_timer(void *arg){
    uint32_t gpio_num=(uint32_t)arg;
    led_state^=1;
    gpio_set_level(gpio_num,led_state);
    ESP_LOGI(TAG,"led_state now: %s",led_state?"on":"off");
}

// callback ISR đọc nút nhấn
void IRAM_ATTR button_cb_iram(void *arg){
    uint32_t gpio_button=(uint32_t)arg;
    uint64_t last=esp_timer_get_time();
    uint64_t debounce=(last-now)/1000;
    // LOGIC DEBOUNCE= NẾU THỜI GIAN ĐỌC CẠNH LÊN NÓ LỚN HƠN THỜI GIAN DEBOUNCE TIME Á --> NHẬN CẠNH ĐÓ LÀM TÍN HIỆU CHUẨN
    // LOGIC: NẾU SAI THÌ RETURN ĐỂ NGẮT LUÔN/ ĐÚNG THÌ NOW=LAST, THỰC HIỆN 2 CHẾ ĐỘ ĐÓ LÀ 1 LÀ BLINK LED, 2 LÀ TẮT BLINK LED
    if (debounce<DEBOUNCE_MS) return;
    now=last;
    if (esp_timer_is_active(timer_handle)){
        esp_timer_stop(timer_handle);
    } else {
        esp_timer_start_periodic(timer_handle,200000);
    }
}

// có thể phải set uart hay cái hàm gì đó ở đây

void app_main(void)
{
    // CẤU HÌNH: SET CHÂN GPIO/ BUTTON PIN
    // gpio_pin
    gpio_config_t gpio_pin_io={
        .pin_bit_mask=(1ULL<<GPIO_PIN),
        .mode=GPIO_MODE_OUTPUT,
        .intr_type=GPIO_INTR_DISABLE,
        .pull_down_en=GPIO_PULLDOWN_DISABLE,
        .pull_up_en=GPIO_PULLUP_DISABLE,
    };
    gpio_config(&gpio_pin_io);
    // button_pin
    gpio_config_t button_pin_io={
        .pin_bit_mask=(1ULL<<BUTTON_PIN),
        .mode=GPIO_MODE_INPUT,
        .intr_type=GPIO_INTR_NEGEDGE,
        .pull_down_en=GPIO_PULLDOWN_DISABLE,
        .pull_up_en=GPIO_PULLUP_ENABLE,
    };
    gpio_config(&button_pin_io);
    // KHAI BÁO: ESP_TIMER CỦA GPIO_LED
    esp_timer_create_args_t gpio_pin_cb={
        .callback=gpio_cb_esp_timer,
        .arg=(void *)GPIO_PIN,
        .name="blink_timer",
    };
    // KHAI BÁO: HANDLE vào từ callback bên trên và ISR set vào để chạy periodic 
    esp_timer_create(&gpio_pin_cb,&timer_handle);
    // KHAI BÁO: ISR CHO CHÂN BUTTON
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_PIN,button_cb_iram,(void *)BUTTON_PIN);
    // ACTION LOOP: ACTION + DEBUG BẰNG UART
    while(1){
        //DEBUG bằng uart
        //Button count++
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
