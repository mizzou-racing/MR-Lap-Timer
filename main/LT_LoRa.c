#include "LT_LoRa.h"
#include "Lap_Time.h"

static const char *TAG = "LT_LoRa";

static spi_device_handle_t spi;

LoRaModule * LoRa;

uint64_t channel = 915000000;

int car_num = -1;

int LoRa_Init(spi_config_t* spiConfig, LoRa_config_t* LoRaConfig, uint32_t car)
{
    car_num = car;

    LoRa = LoRa_initialize(spiConfig->mosi, spiConfig->miso, spiConfig->clk, spiConfig->nss, spiConfig->clockspeed, spiConfig->host, &spi, NULL);
    ESP_ERROR_CHECK(LoRa_set_mode(LoRa_MODE_SLEEP, LoRa));
    ESP_ERROR_CHECK(LoRa_set_frequency(LoRaConfig->frequency,LoRa));
    ESP_ERROR_CHECK(LoRa_reset_fifo(LoRa));
    ESP_ERROR_CHECK(LoRa_set_mode(LoRa_MODE_STANDBY,LoRa));
    ESP_ERROR_CHECK(LoRa_set_bandwidth(LoRa, LoRaConfig->BW));
    ESP_ERROR_CHECK(LoRa_set_implicit_header(NULL, LoRa));
    ESP_ERROR_CHECK(LoRa_set_modem_config_2(LoRaConfig->SF,LoRa));
    ESP_ERROR_CHECK(LoRa_set_syncword(0xf3,LoRa));
    ESP_ERROR_CHECK(LoRa_set_preamble_length(8,LoRa));
    ESP_ERROR_CHECK(LoRa_tx_set_pa_config(LoRa_PA_PIN_BOOST,4,LoRa));

    LoRa_tx_header_t header = {
        .enable_crc = true,
        .coding_rate = LoRaConfig->CR
    };
    ESP_ERROR_CHECK(LoRa_tx_set_explicit_header(&header,LoRa));
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    setup_gpio_interrupts(LoRaConfig->DIO0, LoRa, GPIO_INTR_POSEDGE);
    LoRa->Activity_LED = LoRaConfig->Activity_LED;
    ESP_LOGI(TAG,"LoRa init success :))");
    return ESP_OK;
}

void LoRa_Ritual()
{
    TickType_t curr_ticks;

    while (1)
    {
        curr_ticks = xTaskGetTickCount();

        uint8_t data[MAX_MSG_LEN];
        uint16_t len;
        esp_err_t err = compose_LoRa_msg(data, &len, car_num);

        //printf("minutes %d, sec %d, millisec %d\n", data[6], data[7], (data[8] << 8) | data[9]);

        if (err == ESP_OK)
        {
            ESP_ERROR_CHECK(LoRa_tx_set_for_transmission(data, len*8, LoRa));
            ESP_ERROR_CHECK(LoRa_set_mode(LoRa_MODE_TX, LoRa));

            xTaskDelayUntil(&curr_ticks, pdMS_TO_TICKS(20));
        }
    }
}