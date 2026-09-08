#include "Lap_Time.h"
#include "LT_LoRa.h"
#include "ESPNow.h"

#define LoRaLED 6

#define GPIO_PIN 7

#define NSS 10 
#define MOSI 11
#define SPI_CLK 12
#define MISO 13
#define RST 48
#define DIO0PIN 21
#define DIO1 47
#define SPI_SPEED 10000000
#define LORA_DEFAULT_FREQUENCY 915000000

#define MAIN_TIMER false

//Declare global variable
params globalParams;

void app_main(void)
{

    printf("printsomething please");
    spi_config_t spi = {
        .miso = MISO,
        .mosi = MOSI,
        .clk = SPI_CLK,
        .clockspeed = SPI_SPEED,
        .nss = NSS,
        .host = SPI3_HOST,
    };

    LoRa_config_t LoRa = {
        .Activity_LED = LoRaLED,
        .BW = LoRa_BW_500000,
        .CR = LoRa_CR_4_5,
        .SF = LoRa_SF_7,
        .DIO0 = DIO0PIN,
        .frequency = LORA_DEFAULT_FREQUENCY,
    };

    init_espnow();
    
    start_timer();

    timeval lastTime;
    gettimeofday(&lastTime, NULL);
    globalParams.lastTime = lastTime;
    globalParams.pin = GPIO_PIN;

    xTaskCreate(laser_processing_task, "Laser Processing Task", 4096, NULL, 5, NULL);

    #if MAIN_TIMER

    LoRa_Init(&spi, &LoRa, -1);

    gpio_set_up(&globalParams);
    
    xTaskCreate(lap_processing_task, "Lap Processing Task", 4096, &globalParams, 5, NULL);

    Main_Timer_Setup();

    xTaskCreatePinnedToCore(LoRa_Ritual, "LoRa Ritual Task", 8192, NULL, 2, NULL, 0);

    #else
    
    gpio_set_up(&globalParams);

    //xTaskCreate(lap_processing_task, "Lap Processing Task", 4096, &globalParams, 5, NULL);

    Seg_Timer_Setup();
    xTaskCreate(espnow_seg, "Segment Shit", 4096, NULL, 2, NULL);    

    #endif

    while(1) {
        
            vTaskDelay(10000);
    }
}