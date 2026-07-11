#include "hal_gpio.hpp"
#include "power_control.hpp"

using namespace power_control;

extern "C" void app_main(void)
{
    idf_hals::GpioHAL gpio_hal;
    PowerControl pc(gpio_hal, GPIO_NUM_4, false, false);
    pc.init();
}
