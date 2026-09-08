#include "helper.h"

#define GPIO_PIN    35

static const char *TAG = "Main";

// Declare the global variables
params globalParams;


extern "C" void app_main(void) {
   
    // initialize LoRa
    RFM96 radio = init_LoRa();

    // Initialize ESP-NOW
    init_esp_now();

    timer sinceLastLap;
    start_timer(&sinceLastLap);
    
    struct timeval lastTime;
    gettimeofday(&lastTime, NULL);
    globalParams = {sinceLastLap,lastTime,GPIO_PIN};
    
    //gpio set up times::
    gpio_set_up(&globalParams);

    // The receiver doesn't need to do anything in the loop, the callback handles incoming messages
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000)); // Keep the task alive
        send_LoRa_msg(&radio,&globalParams);
    }
}
