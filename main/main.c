#include "Lap_Time.h"
#include "ESPNow.h"
#include "esp_pm.h"

// No longer used
    // #define LoRaLED 6
    // #define NSS 10 
    // #define MOSI 11
    // #define SPI_CLK 12
    // #define MISO 13
    // #define RST 48
    // #define DIO0PIN 21
    // #define DIO1 47
    // #define SPI_SPEED 10000000
    // #define LORA_DEFAULT_FREQUENCY 915000000
    // SPI is initialized in SPI module if needed
    // Pins are defined as macros above (NSS, MOSI, SPI_CLK, MISO)


void app_main(void) {
    // Configure power management: allow frequency scaling to reduce power when idle
    esp_pm_config_t pm_config = {
        .max_freq_mhz = 240,        // Maximum frequency for performance
        .min_freq_mhz = 80,         // Minimum frequency during idle (significant power savings)
        .light_sleep_enable = true  // Enable light sleep mode
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));

    //Setup ESP-NOW and Lap Time modules
    espnow_init();
    Lap_Time_init();

    while(1) {
        vTaskDelay(1000);
    }
}
    