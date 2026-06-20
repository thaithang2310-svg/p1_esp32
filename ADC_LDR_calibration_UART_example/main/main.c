#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_continuous.h"

// DEFINE (có thể tra docs/ Intellignes)
#define adc_pin ADC_CHANNEL_6
#define adc_attenuation ADC_ATTEN_DB_12
#define adc_bitwidth ADC_BITWIDTH_13
#define timer_delay vTaskDelay(pdMS_TO_TICKS(1000))

// CALIBRATION
static bool 

// SAMPLING

void app_main(void)
{
    // Cấu hình ADC_int 1 lần 
    // Cấu hình dùng ADC_channel nào 
    /* 
    Đưa 2 cái thằng cấu hình trên vào 2 handle riêng biệt, mỗi thằng làm 1 nhiệm vụ riêng: 
    + 1 thằng đọc oneshot để đọc "raw"
    + 1 thằng nhờ "raw" này mới chuyển thành "mv_voltage" mà ta cần dùng
    */
}
