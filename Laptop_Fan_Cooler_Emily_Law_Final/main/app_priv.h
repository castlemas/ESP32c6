#pragma once
#include <stdint.h>
#include <stdbool.h>

#define DEFAULT_SWITCH_POWER        true
#define DEFAULT_TEMPERATURE         25.0
#define DEFAULT_HUMIDITY            50.0
#define REPORTING_PERIOD            20 /* Seconds */

extern esp_rmaker_device_t *switch_device;
extern esp_rmaker_device_t *temp_sensor_device;
extern esp_rmaker_device_t *humidity_sensor_device;


void app_driver_init(void);
int app_driver_set_state(bool state);
bool app_driver_get_state(void);
float app_get_current_temperature();
float app_get_current_humidity();

void dht_test(void *pvParameters);

extern float g_temperature;
extern float g_humidity;
