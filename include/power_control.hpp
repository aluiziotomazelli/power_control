#pragma once

#include <cstdint>

#include "esp_err.h"

#include "gpio_hal.hpp"
#include "power_control_interface.hpp"

class PowerControl : public IPowerControl
{
public:
    PowerControl(
        IGpioHAL &hal,
        const gpio_num_t gpio,
        const bool inverted_logic = false,
        const bool initial_on = false);

    esp_err_t init() override;
    esp_err_t deinit() override;
    esp_err_t set_drive_capability(gpio_drive_cap_t strength) override;
    esp_err_t turn_on() override;
    esp_err_t turn_off() override;
    esp_err_t toggle() override;
    bool is_on() const override { return is_on_; }
    bool is_initialized() const override { return initialized_; }
    gpio_num_t get_pin() const override { return gpio_; }

private:
    esp_err_t apply_gpio(bool enable);

    IGpioHAL &hal_;
    gpio_num_t gpio_;
    bool inverted_logic_;
    bool initial_on_;

    bool initialized_ = false;
    bool is_on_ = false;
};
