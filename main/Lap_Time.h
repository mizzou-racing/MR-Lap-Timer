#ifndef LAP_TIME_H
#define LAP_TIME_H

#include "shared.h"

//Lap Time Includes
#include <stdio.h>
#include <sys/time.h>
#include "freertos/task.h"
#include "hal/gpio_types.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#define LASER_PIN 7
#define LT_data_len 12

typedef struct timeval lt_timeval;

typedef struct {
    uint64_t microSeconds;
    uint16_t minutes;
    uint8_t seconds;
} timer;


esp_err_t Lap_Time_init();

void lap_processing_task(void* pvParameter);

//Laser handling functions
void laser_processing_task(void* pvParameter);

void laserHandler(void* params);

esp_err_t laser_setup(gpio_num_t pin);

#endif //LAP_TIME_H