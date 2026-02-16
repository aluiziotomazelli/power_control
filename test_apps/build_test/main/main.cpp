#include "esp_log.h"

#include "power_control.hpp"

extern "C" void app_main(void)
{
    ESP_LOGI("main", "Testing component compilation");

    GpioHAL gpio_hal;
    PowerControl pc(gpio_hal, GPIO_NUM_4, false, false);
    pc.init();
}
