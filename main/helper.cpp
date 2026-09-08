#include "helper.h"
#include "ESPHal.h"

#define DEBOUNCE_PERIOD_TICKS 50 / portTICK_PERIOD_MS  // 200 ms debounce

#define LAP_DEBOUNCE_PERIOD_SECONDS 2


static const char *TAG = "ESP-NOW Receiver";

// Updated callback function that receives esp_now_recv_info
void esp_now_recv_cb(const esp_now_recv_info_t *recv_info, const uint8_t *data, int data_len) {
    //xTaskCreate(lap_triggered, "esp_now_send_task", 2048, params, 10, NULL);
    xTaskCreate(lap_processing_task, "lap_processing_task", 4096, NULL, 5, NULL);
    ESP_LOGI(TAG, "Received message: yay");
 }


void init_esp_now() {
     // Initialize the non-volatile storage (NVS)
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Initialize Wi-Fi in station mode
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    // Initialize ESP-NOW
    ESP_ERROR_CHECK(esp_now_init());

    // Register the new callback that uses the updated function signature
    ESP_ERROR_CHECK(esp_now_register_recv_cb(esp_now_recv_cb));
}


void send_LoRa_msg(RFM96* radio,params* data)
{
    timer* timer = &(data->laptimer);
    uint8_t sendArray[17] = {0};
    sendArray[1] = -1;
    sendArray[2] = timer->lap /256;
    sendArray[3] = timer ->lap %256; 
    sendArray[5] = timer->minutes % 256;
    sendArray[7] = timer->seconds;
    for (int i = 0; i < 8; i++) {
        sendArray[9 + i] = (timer->microSeconds >> (i * 8)) & 0xFF;
    }
    ESP_LOGI("recent lap time", "Lap: %d, Time: %d:%02d.%llu", 
             (timer->lap), 
             (timer->minutes), 
             (timer->seconds), 
             (unsigned long long)timer->microSeconds);
    radio->transmit(sendArray,17);
}

void start_timer(timer* timer){
    timer->minutes = 0;
    timer->seconds = 0;
    timer->microSeconds = 0;
    timer->lap = 0;
}

//void lap_triggered(timer * timer,timeval* lastTime){
void lap_triggered(void *param) {
    params *passedparams = ((params *)param);
    timer *timer = &(passedparams->laptimer);
    timeval *lastTime = &(passedparams->lastTime);
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    printf("%lu,  %lu\n",currentTime.tv_usec,lastTime->tv_usec);
    // Convert current and last times to uint64_t
    uint64_t currentTimeUs = (uint64_t)currentTime.tv_sec * 1000000 + currentTime.tv_usec;
    uint64_t lastTimeUs = (uint64_t)lastTime->tv_sec * 1000000 + lastTime->tv_usec;

    // Calculate total elapsed time in microseconds
    uint64_t totalMicroseconds = currentTimeUs - lastTimeUs;

    // Check if totalMicroseconds is negative or too small for a valid lap
    if (totalMicroseconds < 2000000) { // 2 seconds in microseconds
        vTaskDelete(NULL); // Ignore this lap if it's too soon
    }

    // Get the minutes and seconds from totalMicroseconds
    uint16_t minutes = totalMicroseconds / 60000000; // 60 seconds * 1000000 microseconds
    uint8_t seconds = (totalMicroseconds % 60000000) / 1000000;

    // Update the timer
    timer->microSeconds = totalMicroseconds % 1000000; // Remainder as microseconds
    timer->lap++;
    timer->minutes = minutes;
    timer->seconds = seconds;

    // Update lastTime for the next lap
    lastTime->tv_sec = currentTime.tv_sec;
    lastTime->tv_usec = currentTime.tv_usec;

    ESP_LOGI("updated lap time", "Lap: %d, Time: %d:%02d.%06llu", 
             (timer->lap), 
             (minutes), 
             (seconds), 
             (uint64_t)timer->microSeconds);
    vTaskDelete(NULL);
}

static uint32_t last_interrupt_tick = 0;

static bool laser_tripped = false;  // Track the laser state


// Interrupt Service Routine for the GPIO
void IRAM_ATTR gpioHandler(void *params) {
    last_interrupt_tick = xTaskGetTickCountFromISR(); // Store the tick count at the time of the interrupt
}

void laser_monitor_task(void *pvParameter) {
    params *passed_params = ((params *)pvParameter);
    bool last_state = false;             // Track the last state of the laser
    const TickType_t debounce_period = pdMS_TO_TICKS(20); // Debounce period of 200 ms

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(10)); // Check every 50 ms

        // Read the GPIO pin state
        bool current_state = gpio_get_level((gpio_num_t)passed_params->pin);

        // If laser is tripped (GPIO goes high)
        if (current_state == 1) {
            if (!last_state) {
                laser_tripped = false; // Reset the tripped state
                //ESP_LOGI("Laser Monitor", "Laser triggered");
            }
        } else { 
            // If laser is no longer tripped (GPIO goes low)
            if (last_state) {
                laser_tripped = true;
            }
        }

        // Get current tick count
        TickType_t current_tick = xTaskGetTickCount();

        // Check if laser is tripped and debounce period has passed
        if (!laser_tripped && (current_tick - last_interrupt_tick) > debounce_period) {
            ESP_LOGI("Laser Monitor", "tracking lap time");
            xTaskCreate(lap_processing_task, "lap_processing_task", 4096, NULL, 5, NULL);
            laser_tripped = true; // Avoid further sends until reset
        }

        last_state = current_state; // Update last state for next loop
    }
}

esp_err_t gpio_set_up(params *parameters) {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_ANYEDGE; // Set for any edge
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << parameters->pin);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;

    esp_err_t error = gpio_config(&io_conf);
    gpio_install_isr_service(ESP_INTR_FLAG_EDGE); // Install ISR service
    gpio_isr_handler_add((gpio_num_t)parameters->pin, gpioHandler, (void *)parameters); // Add ISR handler

    // Start the laser monitor task
    xTaskCreate(laser_monitor_task, "laser_monitor_task", 2048, (void *)parameters, 5, NULL);
    return error;
}

void lap_processing_task(void *pvParameter) {
    
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(30)); // Check every 50 ms
        // Wait for lap data from the queue
        
            ESP_LOGI(TAG, "Processing lap data...");
            lap_triggered((void*)(&globalParams));  // Process the lap trigger
        
    }
}