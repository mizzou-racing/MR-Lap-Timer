#include "ESPNow.h"
#include "Lap_Time.h"
#include "esp_now.h"

static const char *TAG = "ESPNow";

uint8_t main_mac[6] = {0xf0, 0x9e, 0x9e, 0x55, 0x15, 0x08};

esp_now_peer_info_t mainInfo = 
{
    .peer_addr[0] = 0xf0,
    .peer_addr[1] = 0x9e,
    .peer_addr[2] = 0x9e,
    .peer_addr[3] = 0x55,
    .peer_addr[4] = 0x15,
    .peer_addr[5] = 0x08,
    .channel = 0,
    .ifidx = WIFI_IF_STA,
    .encrypt = false,
};

esp_now_peer_info_t segInfo = 
{
    
    .peer_addr[0] = 0xf0,
    .peer_addr[1] = 0x9e,
    .peer_addr[2] = 0x9e,
    .peer_addr[3] = 0x55,
    .peer_addr[4] = 0x15,
    .peer_addr[5] = 0x48,
        .channel = 0,
    .ifidx = WIFI_IF_STA,
    .encrypt = false,
};

void init_espnow()
{
    //Initialize non-volatile storage (NVS)
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    //Initialize Wi-Fi in station mode
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());

    //Initialize ESPNow
    ESP_ERROR_CHECK(esp_now_init());

    ESP_LOGI(TAG,"ESPNow init success!!");
}

void Seg_Timer_Setup()
{
    esp_now_add_peer(&mainInfo);
}

void Main_Timer_Setup()
{
    esp_now_add_peer(&segInfo);
    
    //Register the receive callback
    ESP_ERROR_CHECK(esp_now_register_recv_cb(ESPNow_Recv));   
}

void ESPNow_Send_Trigger()
{
    uint8_t segment_id = 1;
    uint8_t msg[] = {segment_id};

    ESP_ERROR_CHECK(esp_now_send(main_mac, msg, 1));
    printf("triggered lib");
    vTaskDelay(pdMS_TO_TICKS(2000));
}

void ESPNow_Recv (const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) 
{
    uint8_t segment_id = data[0];
    ESP_LOGI(TAG, "Segment tripped. Starting lap...");

    lap_triggered(NULL);
}