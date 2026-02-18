#pragma once

#include "gpio_hal_interface.hpp"
#include "power_control_interface.hpp"

// ========================================
// Power Control Implementation
// ========================================

/**
 * @class PowerControl
 * @brief Concrete implementation of IPowerControl using IGpioHAL
 *
 * This class provides a concrete implementation of the power control interface,
 * delegating hardware operations to an injected IGpioHAL instance. This design
 * enables:
 * - Unit testing through dependency injection (mock HAL)
 * - Platform independence
 * - Runtime configuration of GPIO behavior
 *
 * @copydetails IPowerControl
 *
 * @note This implementation is not thread-safe. External synchronization is
 *       required if used from multiple tasks.
 * @see IGpioHAL for hardware abstraction layer requirements
 */
class PowerControl : public IPowerControl
{
public:
    /**
     * @brief Construct a new Power Control instance
     *
     * @param hal Reference to the HAL implementation for hardware access
     * @param gpio GPIO pin number to control
     * @param inverted_logic true = active LOW, false = active HIGH
     * @param initial_on Initial logical state after init()
     *
     * @note The component is not initialized until init() is called
     * @warning The GPIO pin must support output mode on the target hardware
     */
    PowerControl(
        IGpioHAL &hal,
        const gpio_num_t gpio,
        const bool inverted_logic = false,
        const bool initial_on = false);

    /// @copydoc IPowerControl::init()
    esp_err_t init() override;

    /// @copydoc IPowerControl::deinit()
    esp_err_t deinit() override;

    /// @copydoc IPowerControl::set_drive_capability()
    esp_err_t set_drive_capability(gpio_drive_cap_t strength) override;

    /// @copydoc IPowerControl::turn_on()
    esp_err_t turn_on() override;

    /// @copydoc IPowerControl::turn_off()
    esp_err_t turn_off() override;

    /// @copydoc IPowerControl::toggle()
    esp_err_t toggle() override;

    /// @copydoc IPowerControl::is_on()
    bool is_on() const override { return is_on_; }

    /// @copydoc IPowerControl::is_initialized()
    bool is_initialized() const override { return initialized_; }

    /// @copydoc IPowerControl::get_pin()
    gpio_num_t get_pin() const override { return gpio_; }

private:
    /**
     * @brief Apply a logical state to the GPIO
     *
     * Internal helper that handles the logic-to-physical level conversion
     * based on inverted_logic_ setting.
     *
     * @param enable Desired logical state (true = ON, false = OFF)
     * @return esp_err_t Result of the operation
     *
     * @note Updates is_on_ only on successful HAL operation
     */
    esp_err_t apply_gpio(bool enable);

    IGpioHAL &hal_;       ///< HAL instance for hardware access
    gpio_num_t gpio_;     ///< GPIO pin number
    bool inverted_logic_; ///< true = active LOW, false = active HIGH
    bool initial_on_;     ///< Initial state to apply after init

    bool initialized_ = false; ///< Initialization state
    bool is_on_ = false;       ///< Current logical state
};