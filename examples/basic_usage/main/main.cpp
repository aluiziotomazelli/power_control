#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "power_control.hpp"
#include "gpio_hal.hpp"

// Tag for logging
static const char *TAG = "EXAMPLE";

// Define the GPIO pin to be used.
// GPIO 2 is the default internal LED for many ESP32 DevKits.
#define POWER_CONTROL_PIN GPIO_NUM_2

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting Power Control basic usage example...");

    /**
     * @brief Dependency Injection - HAL Instance
     *
     * The PowerControl component uses an abstraction layer (HAL) to interact with the hardware.
     * Here we instantiate the concrete implementation for ESP-IDF (GpioHAL).
     * This instance will be injected into the PowerControl constructor.
     *
     * This design allows you to easily swap the hardware implementation or use a Mock
     * during unit testing.
     */
    GpioHAL hal;

    /**
     * @brief PowerControl Instance
     *
     * We create the control instance by passing:
     * 1. The HAL instance (&hal)
     * 2. The GPIO pin number (POWER_CONTROL_PIN)
     * 3. Logic type (false = normal logic, where HIGH = ON)
     * 4. Initial state (false = starts OFF after init)
     */
    PowerControl led_power(hal, POWER_CONTROL_PIN, false, false);

    while (1) {
        ESP_LOGI(TAG, "--- New Cycle ---");

        /**
         * @brief Component Initialization
         *
         * Before using any method, we must initialize the hardware.
         * The init() method configures the GPIO as output and sets it to the initial state.
         */
        esp_err_t err = led_power.init();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to initialize power control: %s", esp_err_to_name(err));
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }

        ESP_LOGI(TAG, "Turning ON...");
        led_power.turn_on();

        /**
         * NOTE: If this were a sensor, normally here you would wait for a 'warmup time'
         * if required by the datasheet before performing a reading.
         *
         * vTaskDelay(pdMS_TO_TICKS(warmup_ms));
         * sensor.read();
         */

        // Let it stay ON for 2 seconds (simulating sensor activity or just for visual blink)
        vTaskDelay(pdMS_TO_TICKS(2000));

        ESP_LOGI(TAG, "Turning OFF...");
        led_power.turn_off();

        // Wait for a while
        vTaskDelay(pdMS_TO_TICKS(1000));

        /**
         * @brief Component Deinitialization
         *
         * At the end of the operation, we can deinitialize the component.
         * This returns the GPIO to a safe state (Low and then disabled/high-impedance).
         * Useful in low-power applications to ensure no current leakage occurs
         * through the pin when it's not in use.
         */
        ESP_LOGI(TAG, "Deinitializing...");
        led_power.deinit();

        // Wait before starting the next cycle
        ESP_LOGI(TAG, "Cycle completed. Waiting 3 seconds...");
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}
