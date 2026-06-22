#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_continuous.h"
#include "esp_log.h"

// DEFINE (có thể tra docs/ Intellignes)
#define ADC_CHANNEL ADC_CHANNEL_6 // GPIO 34
#define ADC_ATTEN ADC_ATTEN_DB_12 // Dải đo từ 0 - 3v3
#define SAMPLING_COUNT 10 // Số mẫu lấy trong 1 lần đo
#define READ_INTERVAL_MS 500 // Đọc mỗi 500ms

static const char *TAG="ADC_LDR";

// CHECKING CALIBRATION EFUSE -> Chưa biết parameter -> Checking bằng handle coi thử có ESP_OK không, có thì dùng efuse, không thì dùng mặc định Vref 
static bool checking_calibration(adc_atten_t atten, 
                                adc_bitwidth_t bitwidth, 
                                adc_channel_t adc_channel, 
                                adc_cali_handle_t *out_handle){
    adc_cali_handle_t handle=NULL;
    bool calibration=false; // Set flag
    #if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    adc_cali_line_fitting_config_t cali_config={
        .atten=atten,
        .bitwidth=ADC_BITWIDTH_DEFAULT,
        .unit_id=adc_channel, // Dòng này dùng để làm gì? Cả parameter của nó nữa? Dùng ADC1 hay ADC2 ấy
    };
    if (adc_cali_create_scheme_line_fitting(&cali_config,&handle)==ESP_OK){
        ESP_LOGI(TAG,"Calibration line fitting is correct!");
        calibration=true;
    }
    #endif
    if (!calibration){
        ESP_LOGI(TAG,"Dont have data of eFuse, use default Vref!");
    };
    *out_handle=handle;
    return calibration;
}

// SAMPLING: dùng để lấy giá trị nhiều lần trong thời gian ngắn -> TBC -> Ra giá trị chuẩn
int sampling(adc_oneshot_unit_handle_t handle_oneshot, adc_channel_t adc_channel){
    // int voltage=0; 
    int raw=0;
    int sum=0;
    for (int i=0;i<SAMPLING_COUNT;i++){
        adc_oneshot_read(handle_oneshot, adc_channel,&raw);
        sum+=raw;
        vTaskDelay(1);
    };
    return sum/SAMPLING_COUNT;
}

void app_main(void)
{
    // Cấu hình
    // Cấu hình oneshot
    adc_oneshot_unit_handle_t init_oneshot_handle=NULL;
    adc_oneshot_unit_init_cfg_t init_oneshot_config={
        .unit_id=ADC_UNIT_1,
        .clk_src=ADC_DIGI_CLK_SRC_DEFAULT, // Tuỳ chọn xung nhịp tuỳ chức năng lấy mẫu của ESP32 
        .ulp_mode=ADC_ULP_MODE_DISABLE,
    };
    adc_oneshot_new_unit(&init_oneshot_config,&init_oneshot_handle);
    
    // Cấu hình channel
    adc_oneshot_chan_cfg_t oneshot_channel={
        .atten=ADC_ATTEN,
        .bitwidth=ADC_BITWIDTH_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(init_oneshot_handle,ADC_CHANNEL,&oneshot_channel));
    // Cấu hình calibration 
    adc_cali_handle_t cali_handle=NULL;
    bool calibrated=checking_calibration(ADC_ATTEN,ADC_BITWIDTH_12,ADC_CHANNEL,&cali_handle);
    // Vòng lặp
    while(1){
        int raw=sampling(init_oneshot_handle, ADC_CHANNEL);
        int voltage_mv=0;
        if(calibrated){
            adc_cali_raw_to_voltage(cali_handle,raw,&voltage_mv);
        } else {
            voltage_mv=(int)((float)raw/4095.0f*3100.0f);
        };
        int light_percent=100-(int)((float)raw/4095.0f*100);

        ESP_LOGI(TAG,"Raw: %4d | Voltage: %4d mV | Sang: %3d%%",raw, voltage_mv, light_percent);
        vTaskDelay(pdMS_TO_TICKS(READ_INTERVAL_MS));
    }
}
