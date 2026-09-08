#ifndef ESPNow_H
#define ESPNow_H

#include "shared.h"

//Wifi and ESP-NOW includes
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_now.h"

#define ESP_NOW_ETH_ALEN 6
#define MAX_MSG_LEN 256
#define PADDOCK_CHANNEL 11
#define LT_data_len 12

// Function declarations
/**
 * @brief Initialize ESP-NOW wireless communication
 * @return ESP_OK on success, error code on failure
 */
int espnow_init();

/**
 * @brief Send ESP-NOW data to a specific peer
 * @param dest_mac Destination MAC address (6 bytes)
 * @param data Pointer to data buffer
 * @param length Data length in bytes (max 250)
 * @return ESP_OK on success, error code on failure
 */
int espnow_send(const uint8_t *dest_mac, uint8_t* data, size_t length);

/**
 * @brief Sends data via ESP-NOW to the paddock to let it know a lap time should be recorded
 */
void trigger_paddock();

#endif // ESPNow_H