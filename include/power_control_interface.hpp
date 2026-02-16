#pragma once

#include "esp_err.h"
#include <cstdint>
#include "driver/gpio.h"

class IPowerControl
{
public:
    virtual ~IPowerControl() = default;

    virtual esp_err_t init() = 0;
    virtual esp_err_t deinit() = 0;
    virtual esp_err_t set_drive_capability(gpio_drive_cap_t strength) = 0;
    virtual esp_err_t turn_on() = 0;
    virtual esp_err_t turn_off() = 0;
    virtual esp_err_t toggle() = 0;
    virtual bool is_on() const = 0;
    virtual bool is_initialized() const = 0;
    virtual gpio_num_t get_pin() const = 0;
};