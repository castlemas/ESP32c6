/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <sdkconfig.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

#include <iot_button.h>
#include <esp_rmaker_core.h>
#include <esp_rmaker_standard_types.h>
#include <esp_rmaker_standard_params.h>

#include <app_reset.h>
#include <ws2812_led.h>
#include "app_priv.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <inttypes.h>


/* This is the button that is used for toggling the power */
#define BUTTON_GPIO          CONFIG_EXAMPLE_BOARD_BUTTON_GPIO
#define BUTTON_ACTIVE_LEVEL  0
/* This is the GPIO on which the power will be set */
#define OUTPUT_GPIO    CONFIG_EXAMPLE_OUTPUT_GPIO

/* These values correspoind to H,S,V = 120,100,10 */
#define DEFAULT_RED     0
#define DEFAULT_GREEN   25
#define DEFAULT_BLUE    0

#define WIFI_RESET_BUTTON_TIMEOUT       3
#define FACTORY_RESET_BUTTON_TIMEOUT    10

/* This is the GPIO for the moisture sensor and water pump*/
#define MOISTURE_SENSOR_ADC_CHANNEL ADC1_CHANNEL_6
#define WATER_PUMP_RELAY_GPIO 3

static bool g_power_state = DEFAULT_SWITCH_POWER;
static float g_temperature = DEFAULT_TEMPERATURE;
float moisture;
static TimerHandle_t sensor_timer;

float app_driver_read_moisture() {
    // Example ADC reading
    float moisture = adc1_get_raw(MOISTURE_SENSOR_ADC_CHANNEL);
    ESP_LOGI("MOISTURE_READ", "Moisture level read: %f", moisture);
    return moisture;
}

static void app_sensor_update(TimerHandle_t handle)
{
    // read the moisture sensor value
    moisture = app_driver_read_moisture();

    // try see if this helps with triggering the water pump
    bool desired_state = (moisture > 1500);
    if (desired_state != g_power_state) {
        app_driver_set_state(desired_state);
    }

    esp_rmaker_param_update_and_report(
                esp_rmaker_device_get_param_by_type(temp_sensor_device, ESP_RMAKER_PARAM_TEMPERATURE),
                esp_rmaker_float(moisture));
}

float app_get_current_moisture()
{
    return moisture;
}

void app_driver_set_water_pump(bool on) {
    ESP_LOGI("APP_DRIVER_WATER_PUMP", "Water pump turned %s", on ? "on" : "off");
    gpio_set_level(WATER_PUMP_RELAY_GPIO, on ? 1 : 0);

    if (on) {
        if (moisture > 1500) { // Confirm that moisture levels still require pumping
            // Run water pump for 10 seconds then turn it off
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            gpio_set_level(WATER_PUMP_RELAY_GPIO, 0);
            ESP_LOGI("APP_DRIVER_WATER_PUMP", "Water pump turned off after 10 seconds");
        } else {
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            gpio_set_level(WATER_PUMP_RELAY_GPIO, 0);
            ESP_LOGI("APP_DRIVER_WATER_PUMP", "Water pump turned off after 5 seconds");
        }
        // Update the global switch state to reflect the water pump is off
        g_power_state = false;  // Assuming g_power_state is your global variable tracking the switch state

        // Report the new state to the RainMaker cloud
        esp_rmaker_param_update_and_report(
            esp_rmaker_device_get_param_by_type(switch_device, ESP_RMAKER_PARAM_POWER),
            esp_rmaker_bool(g_power_state));
    }
    else {
        gpio_set_level(WATER_PUMP_RELAY_GPIO, 0);
        ESP_LOGI("APP_DRIVER_WATER_PUMP", "Water pump confirmed off");
    }
}



esp_err_t app_sensor_init(void)
{
    g_temperature = DEFAULT_TEMPERATURE;
    sensor_timer = xTimerCreate("app_sensor_update_tm", (REPORTING_PERIOD * 1000) / portTICK_PERIOD_MS,
                            pdTRUE, NULL, app_sensor_update);
    if (sensor_timer) {
        xTimerStart(sensor_timer, 0);
        return ESP_OK;
    }
    return ESP_FAIL;
}

static void app_indicator_set(bool state)
{
    if (state) {
        ws2812_led_set_rgb(DEFAULT_RED, DEFAULT_GREEN, DEFAULT_BLUE);
    } else {
        ws2812_led_clear();
    }
}

static void app_indicator_init(void)
{
    ws2812_led_init();
    app_indicator_set(g_power_state);
}

static void push_btn_cb(void *arg)
{
    ESP_LOGI("SWITCH_CB", "Switch callback triggered");
    bool new_state = !g_power_state;    // Toggles the state
    app_driver_set_state(new_state);
    esp_rmaker_param_update_and_report(
                esp_rmaker_device_get_param_by_type(switch_device, ESP_RMAKER_PARAM_POWER),
                esp_rmaker_bool(new_state));
}

static void set_power_state(bool target)
{
    gpio_set_level(OUTPUT_GPIO, target);
    app_indicator_set(target);
}

void app_driver_init()
{
    button_handle_t btn_handle = iot_button_create(BUTTON_GPIO, BUTTON_ACTIVE_LEVEL);
    if (btn_handle) {
        /* Register a callback for a button tap (short press) event */
        iot_button_set_evt_cb(btn_handle, BUTTON_CB_TAP, push_btn_cb, NULL);
        /* Register Wi-Fi reset and factory reset functionality on same button */
        app_reset_button_register(btn_handle, WIFI_RESET_BUTTON_TIMEOUT, FACTORY_RESET_BUTTON_TIMEOUT);
    }

    /* Configure power */
    gpio_config_t io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 1,
    };
    io_conf.pin_bit_mask = ((uint64_t)1 << OUTPUT_GPIO);
    /* Configure the GPIO */
    gpio_config(&io_conf);
    app_indicator_init();
    app_sensor_init();

    // Start of new code integration
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(MOISTURE_SENSOR_ADC_CHANNEL, ADC_ATTEN_DB_12);
    esp_rom_gpio_pad_select_gpio(WATER_PUMP_RELAY_GPIO);
    gpio_set_direction(WATER_PUMP_RELAY_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(WATER_PUMP_RELAY_GPIO, 0); // Active-high relay, set it off initially
    // End of new code integration
}

int IRAM_ATTR app_driver_set_state(bool state)
{
    if(g_power_state != state) {
        g_power_state = state;
        set_power_state(g_power_state);
        
        // Control the water pump based on the switch state
        ESP_LOGI("WATER_PUMP", "Water pump turned %s", state ? "on" : "off");
        app_driver_set_water_pump(state);
        ESP_LOGI("WATER_PUMP", "Water pump done");
        
    }
    return ESP_OK;
}

bool app_driver_get_state(void)
{
    return g_power_state;
}
