#include <stdio.h> // Khởi tạo để có các lệnh cơ bản của C trong monitor -> Nếu không có thì print ra monitor không được và buộc phải dùng LOGX
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h" // thư viện timer để callback -> Nếu không có thì ta không thể làm việc khác lúc chạy timer được, và nếu không có buộc phải dùng VtaskDelay bị treo CPu
#include "driver/gpio.h" // Setup thư viện cho gpio -> nếu không set là không có giao thức cho cả hệ thống (các giao thức khác cũng là driver/)
#include "esp_log.h" // Hàm này dùng để INFO lên monitor để theo dõi trạng thái để debug dữ liệu bị sai 

static volatile uint8_t led_state=0; // set trạng thái ban đầu của led -> Ví dụ khi không có? Không xác định được trạng thái hiện tại của chân led để togggle bình thường
const char *TAG="BLINK_LED"; // Tại sao ở đây không dùng define tag là cái giá trị này luôn cho khoẻ? 

// Setup hàm callback mà quên mất cái hàm callback đó là kiểu dữ liệu gì rồi??
static void blink_callback(void *arg){
    led_state^=1; 
    gpio_set_level(GPIO_NUM_2,led_state);
    ESP_LOGI(TAG,"LED: %s",led_state==1?"ON":"OFF"); // Tại sao ở đây truyền địa chỉ mà C vẫn hiểu được để thực hiện code? -> STRING nhận biết/ Char nhận biết
}

void app_main(){
     gpio_config_t gpio_io={
        .pin_bit_mask=(1ULL<<GPIO_NUM_2), 
        .mode=GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type=GPIO_INTR_DISABLE // Hàm ngắt? -> Chưa hiểu nguyên lý hoạt động cho lắm -> Nếu không có thì sao và ảnh hưởng gì không?
     };
     gpio_config(&gpio_io);

    esp_timer_handle_t timer_handle;
    esp_timer_create_args_t prod_callback={
        .callback=&blink_callback,
        .name="GPIO2_blink",
    };
    esp_timer_create(&prod_callback,&timer_handle); // Chỗ này sẽ có 2 biến vào, đó là timer_handle và hàm callback làm công việc
    esp_timer_start_periodic(timer_handle,500000); // Tại sao chỗ này periodic lại thực hiện trên timer_handle trực tiếp chứ không phải tại địa chỉ?
    // Nếu truyền địa chỉ vào thì sao? -> vì timer_handle đã là handle (con trỏ) rồi, lấy địa chỉ của nó là sai tầng.
    while(1){
        vTaskDelay(pdMS_TO_TICKS(1000));
    };
}