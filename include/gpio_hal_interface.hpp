#pragma once

#include "esp_err.h"
#include "driver/gpio.h"

class IGpioHAL
{
public:
    virtual ~IGpioHAL() = default;

    virtual esp_err_t reset_pin(const gpio_num_t pin) = 0;
    virtual esp_err_t config(const gpio_config_t &config) = 0;
    virtual esp_err_t set_level(const gpio_num_t pin, bool level) = 0;
    virtual esp_err_t set_drive_capability(const gpio_num_t gpio_num, gpio_drive_cap_t strength) = 0;
};