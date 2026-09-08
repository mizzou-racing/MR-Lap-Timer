#include <string.h>
#include <RadioLib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_task_wdt.h"
#include "esp_log.h"
#include "esp_now.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "hal/gpio_types.h"
#include "driver/gpio.h"
#include "esp_sntp.h"

#define SCK 18
#define MISO 19
#define MOSI 23
#define NSS 5
#define DIO0 32
#define NRST 27
#define DIO1 34

typedef struct {
    uint16_t minutes;
    uint8_t seconds;
    uint64_t microSeconds;
    uint16_t lap;
}timer;

typedef struct {
    timer laptimer;
    timeval lastTime;
    int pin;
} params;

extern params globalParams;


#ifdef __cplusplus
extern "C" {
#endif


void esp_now_recv_cb(const esp_now_recv_info_t *mac_addr, const uint8_t *data, int data_len);
void init_esp_now();
RFM96 init_LoRa();
void send_LoRa_msg(RFM96*,params*);
void start_timer(timer*);
void lap_triggered(void* par);


void esp_now_set_up(esp_now_peer_info_t *);


esp_err_t gpio_set_up(params*);

void IRAM_ATTR gpioHandler(void* );

void check_laser_state(int pin);

void laser_monitor_task(void *pvParameter);

void lap_processing_task(void *pvParameter);
    
#ifdef __cplusplus
}
#endif