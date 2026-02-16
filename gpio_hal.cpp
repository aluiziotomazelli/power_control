#include "gpio_hal.hpp"

esp_err_t GpioHAL::reset_pin(const gpio_num_t pin)
{
    return gpio_reset_pin(pin);
}

esp_err_t GpioHAL::config(const gpio_config_t &config)
{
    return gpio_config(&config);
}

esp_err_t GpioHAL::set_level(const gpio_num_t pin, bool level)
{
    return gpio_set_level(pin, level);
}

esp_err_t GpioHAL::set_drive_capability(const gpio_num_t gpio_num, gpio_drive_cap_t strength)
{
    return gpio_set_drive_capability(gpio_num, strength);
}