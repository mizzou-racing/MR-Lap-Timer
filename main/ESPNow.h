#ifndef ESPNow_H
#define ESPNow_H

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "nvs_flash.h"
#include "esp_random.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_now.h"
#include "esp_crc.h"

#define ESP_NOW_ETH_ALEN 6

void init_espnow();

void Main_Timer_Setup();
void Seg_Timer_Setup();


void ESPNow_Send_Trigger();

esp_err_t esp_now_send(const uint8_t *mac_addr, const uint8_t *data, size_t len);

void ESPNow_Recv(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len); 

#endif //ESPNow_IF