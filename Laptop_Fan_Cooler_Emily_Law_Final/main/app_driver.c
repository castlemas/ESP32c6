#include <stdio.h>
#include <dht.h>
#include <sdkconfig.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include "esp_http_client.h"
#include <iot_button.h>
#include <esp_rmaker_core.h>
#include <esp_rmaker_standard_types.h>
#include <esp_rmaker_standard_params.h>

#include <app_reset.h>
#include <ws2812_led.h>
#include "app_priv.h"

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

#define SENSOR_TYPE DHT_TYPE_DHT11 
#define CONFIG_EXAMPLE_DATA_GPIO 1

static bool g_power_state = DEFAULT_SWITCH_POWER;
float g_temperature = DEFAULT_TEMPERATURE;
float g_humidity = DEFAULT_HUMIDITY;
static TimerHandle_t sensor_timer;

/*static void app_sensor_update(TimerHandle_t handle)
{
    dht_test(NULL);
    printf("appsensorupdate\n");
    esp_rmaker_param_update_and_report(
                esp_rmaker_device_get_param_by_type(temp_sensor_device, "Temperature"),
                esp_rmaker_float(g_temperature));

    esp_rmaker_param_update_and_report(
                esp_rmaker_device_get_param_by_type(temp_sensor_device1, "Humidity"),
                esp_rmaker_float(g_humidity));

}*/

static void app_sensor_update(TimerHandle_t handle) {
    // Trigger sensor reading update
    dht_test(NULL);

    // Log update event
    printf("Sensor update triggered\n");

    // Fetch and check the temperature parameter handle
    esp_rmaker_param_t *temp_param = esp_rmaker_device_get_param_by_type(temp_sensor_device, ESP_RMAKER_PARAM_TEMPERATURE);
    if (temp_param) {
        esp_rmaker_param_update_and_report(temp_param, esp_rmaker_float(g_temperature));
    } else {
        printf("Temperature parameter handle is NULL\n");
    }

    // Fetch and check the humidity parameter handle
    esp_rmaker_param_t *humidity_param = esp_rmaker_device_get_param_by_type(humidity_sensor_device, ESP_RMAKER_PARAM_TEMPERATURE);
    if (humidity_param) {
        esp_rmaker_param_update_and_report(humidity_param, esp_rmaker_float(g_humidity));
    } else {
        printf("Humidity parameter handle is NULL\n");
    }
}



float app_get_current_temperature()
{
    printf("get_current_temp\n");
    return g_temperature;
}
float app_get_current_humidity()
{
    printf("get_current_humidity\n");
    return g_humidity;
}

esp_err_t app_sensor_init(void)
{
    g_temperature = DEFAULT_TEMPERATURE;
    g_humidity = DEFAULT_HUMIDITY;
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
    bool new_state = !g_power_state;
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
}


void dht_test(void*pvParameters) {
    float temperature, humidity;
    (void)pvParameters;
    

    
        if (dht_read_float_data(SENSOR_TYPE, CONFIG_EXAMPLE_DATA_GPIO, &humidity, &temperature) == ESP_OK) {
            printf("Humidity: %.1f%% Temp: %.1fC\n", humidity, temperature);
            g_temperature = temperature; // Update the global temperature variable
            g_humidity = humidity;
        } else {
            printf("Could not read data from sensor\n");
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    
}


int IRAM_ATTR app_driver_set_state(bool state)
{
    if(g_power_state != state) {
        g_power_state = state;
        set_power_state(g_power_state);
    }
    return ESP_OK;
}

bool app_driver_get_state(void)
{
    return g_power_state;
}
