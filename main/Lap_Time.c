#include "Lap_Time.h"
#include "ESPNow.h"

#define TAG ((const char *const) "Lap_Time")

//Laser State Variables
static SemaphoreHandle_t laser_signal;
StaticSemaphore_t laser_signal_buffer;
static volatile bool laser_tripped = false;
static volatile uint32_t last_interrupt_tick = 0;


//Initialization function for Lap Time module
esp_err_t Lap_Time_init() {

    // Initialize laser handling
    esp_err_t err = laser_setup(LASER_PIN); 
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to setup laser: %s", esp_err_to_name(err));
        return err;
    }

    // Create tasks for processing lap times and laser signals
    xTaskCreate(lap_processing_task, "LapProcessingTask", 4096, NULL, 5, NULL);
    xTaskCreate(laser_processing_task, "LaserProcessingTask", 4096, NULL, 5, NULL);

    return ESP_OK;
}

void lap_processing_task(void* pvParameter) {  
   while (1) {
        // Small delay to debounce
        vTaskDelay(pdMS_TO_TICKS(25));
        
        // Only process if this is a valid lap trigger (not too soon after last)
        uint32_t current_tick = xTaskGetTickCount();
        if (current_tick - last_interrupt_tick > pdMS_TO_TICKS(25)) {
            if(!laser_tripped)
            {
                trigger_paddock();
                laser_tripped = true;
            }
        }
    }
}

void laser_processing_task(void* pvParameter) {
    while (1) {
        // Wait for laser signal to be triggered
        xSemaphoreTake(laser_signal, portMAX_DELAY);
        last_interrupt_tick = xTaskGetTickCount(); 
        laser_tripped = false;
        
    }
}

//Laser handling functions
void IRAM_ATTR laserHandler(void* params) {
    // static uint32_t last_isr_time = 0;
    // uint32_t current_time = xTaskGetTickCountFromISR();
    
    // // Debounce ISR calls (minimum 100ms between interrupts)
    // if (current_time - last_isr_time > pdMS_TO_TICKS(100)) {
    //     last_isr_time = current_time;
    //     xSemaphoreGiveFromISR(laser_signal, NULL);
    // }
    xSemaphoreGive(laser_signal);
}

esp_err_t laser_setup(gpio_num_t pin) {
    // Create the semaphore for laser signal handling
    laser_signal = xSemaphoreCreateBinaryStatic(&laser_signal_buffer);
    
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_NEGEDGE;  // Trigger once on falling edge (high to low)
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << pin);
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;

    esp_err_t err = gpio_config(&io_conf);
    if(err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure GPIO: %s", esp_err_to_name(err));
        return err;
    }

    gpio_install_isr_service(ESP_INTR_FLAG_EDGE); // Use edge interrupts for NEGEDGE
    gpio_isr_handler_add(pin, laserHandler, NULL); // Add ISR handler

    return err;
}