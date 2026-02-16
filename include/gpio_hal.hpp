#pragma once

#include "gpio_hal_interface.hpp"

class GpioHAL : public IGpioHAL
{
public:
    GpioHAL() = default;
    ~GpioHAL() override = default;
    esp_err_t reset_pin(const gpio_num_t pin) override;
    esp_err_t config(const gpio_config_t &config) override;
    esp_err_t set_level(const gpio_num_t pin, const bool level) override;
    esp_err_t set_drive_capability(gpio_num_t gpio_num, gpio_drive_cap_t strength) override;
};