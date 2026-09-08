  #ifndef LT_LORA_H
  #define LT_LORA_H

  #include "LoRa.h"
  
  #define MAX_MSG_LEN 256

  typedef struct {
    int mosi;
    int miso;
    int clk;
    int nss;
    uint32_t clockspeed;
    spi_host_device_t host;
  } spi_config_t;

  typedef struct {
    LoRa_bw_t BW;
    LoRa_sf_t SF;
    LoRa_cr_t CR;
    uint64_t frequency;
    gpio_num_t DIO0;
    gpio_num_t Activity_LED;
  } LoRa_config_t;

int LoRa_Init(spi_config_t* spiConfig, LoRa_config_t* LoRaConfig, uint32_t car);

void LoRa_Ritual();

  #endif //LT_LORA_H