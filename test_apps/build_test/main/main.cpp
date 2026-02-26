#include "gpio_hal.hpp"
#include "power_control.hpp"

extern "C" void app_main(void)
{
    GpioHAL gpio_hal;
    PowerControl pc(gpio_hal, GPIO_NUM_4, false, false);
    pc.init();
}
