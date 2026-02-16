#include "esp_err.h"

#define LOG_LOCAL_LEVEL ESP_LOG_INFO
#include "esp_log.h"

#include "gpio_hal.hpp"
#include "power_control.hpp"

static const char *TAG = "PowerControl";

PowerControl::PowerControl(IGpioHAL &hal, const gpio_num_t gpio, const bool inverted_logic, const bool initial_on)
    : hal_(hal)
    , gpio_(gpio)
    , inverted_logic_(inverted_logic)
    , initial_on_(initial_on)

{
}

esp_err_t PowerControl::init()
{
    if (initialized_) {
        return ESP_OK;
    }

    ESP_LOGI(
        TAG,
        "Initializing power control on GPIO %d (active_%s, initial_%s)",
        gpio_,
        inverted_logic_ ? "true" : "false",
        initial_on_ ? "on" : "off");

    // Reset GPIO before initialization
    esp_err_t ret = hal_.reset_pin(gpio_);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to reset GPIO %d, error: %s", gpio_, esp_err_to_name(ret));
        return ret;
    }

    // Set GPIO as output
    gpio_config_t io_conf = {};
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    io_conf.pin_bit_mask = 1ULL << gpio_;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;

    // Configure GPIO
    ret = hal_.config(io_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure GPIO %d, error: %s", gpio_, esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGD(TAG, "GPIO %d configured successfully", gpio_);

    initialized_ = true;
    ret = initial_on_ ? turn_on() : turn_off();

    ESP_LOGI(TAG, "Power control initialized successfully");

    return ESP_OK;
}

esp_err_t PowerControl::apply_gpio(bool enable)
{
    // Check if the power control is initialized
    if (!initialized_) {
        ESP_LOGE(TAG, "Power control not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    // Set physical level to logic level
    bool level = inverted_logic_ ? !enable : enable;
    esp_err_t ret = hal_.set_level(gpio_, level); // Set GPIO
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set GPIO %d to enable=%d (physical_level=%d)", gpio_, enable, level);
        return ret;
    }
    else {
        is_on_ = enable; // Update internal state
        ESP_LOGD(TAG, "GPIO %d enabled=%d (physical_level=%d)", gpio_, enable, level);
        return ESP_OK;
    }
}

esp_err_t PowerControl::turn_on()
{
    return apply_gpio(true);
}

esp_err_t PowerControl::turn_off()
{
    return apply_gpio(false);
}

esp_err_t PowerControl::toggle()
{
    return apply_gpio(!is_on_);
}

esp_err_t PowerControl::deinit()
{
    if (!initialized_) {
        return ESP_OK;
    }

    // Force GPIO low before deinitialization for safety
    esp_err_t ret;
    ret = hal_.set_level(gpio_, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set GPIO low during deinit");
        return ret;
    }

    // Reset GPIO (returns to high-impedance state)
    ret = hal_.reset_pin(gpio_);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to reset GPIO during deinit");
        return ret;
    }

    initialized_ = false;
    is_on_ = false; // Reset internal state also
    ESP_LOGI(TAG, "Power control deinitialized on GPIO %d", gpio_);

    return ret;
}

esp_err_t PowerControl::set_drive_capability(gpio_drive_cap_t strength)
{
    ESP_LOGD(TAG, "GPIO %d drive capability set to %d ", gpio_, strength);
    return hal_.set_drive_capability(gpio_, strength);
}