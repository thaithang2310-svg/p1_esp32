#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "driver/gptimer.h"

static volatile uint8_t led_state=0;
void timer_cb(void *arg){
    led_state^=1; 
    
}

void app_main(void)
{
    //set gpio
    //esptimer
    //periodic luon
    //gptimer
    //
}
