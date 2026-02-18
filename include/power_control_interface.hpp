#pragma once

#include "esp_err.h"
#include <cstdint>
#include "driver/gpio.h"

// ========================================
// Power Control Interface
// ========================================

/**
 * @interface IPowerControl
 * @brief Interface for GPIO-based power/output control
 *
 * This interface defines the contract for controlling power outputs via GPIO pins.
 * Supports both normal and inverted logic, and provides complete lifecycle management
 * (initialization, operation, deinitialization).
 *
 * The implementation handles the physical GPIO levels while exposing logical ON/OFF
 * states to the application, abstracting away hardware details like active-high vs
 * active-low configurations.
 *
 * @author [github.com/aluiziotomazelli]
 * @version 1.0.0
 * @date 2025
 * @see PowerControl for concrete implementation
 * @see IGpioHAL for hardware abstraction layer
 */
class IPowerControl
{
public:
    virtual ~IPowerControl() = default;

    // ========================================
    // Lifecycle Management
    // ========================================

    /**
     * @brief Initialize the power control hardware
     *
     * Configures the GPIO pin as an output, sets up the hardware, and applies
     * the initial state as specified in the constructor.
     *
     * This method must be called before any other operations (turn_on, turn_off, etc.).
     * The GPIO pin is reset to a safe state before configuration.
     *
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_ARG: GPIO number is invalid or cannot be used as output
     * @return ESP_ERR_INVALID_STATE: component already initialized
     * @return ESP_ERR_NO_MEM: memory allocation failed in HAL layer
     * @return ESP_FAIL: hardware configuration failed
     * @return Other: error codes propagated from the underlying IGpioHAL implementation
     *
     * @note Idempotent: calling multiple times returns ESP_OK without re-initializing
     * @note The GPIO is forced to a known state (low) during initialization for safety
     */
    virtual esp_err_t init() = 0;

    /**
     * @brief Deinitialize the power control
     *
     * Sets the GPIO to a safe state (low) and releases hardware resources.
     * The pin returns to high-impedance state after deinitialization.
     *
     * This method is safe to call even if the component is not initialized.
     * Partial failures during deinitialization are reported but do not prevent
     * the component from being marked as deinitialized.
     *
     * @return ESP_OK on success or if already deinitialized
     * @return ESP_ERR_INVALID_STATE: hardware is in an inconsistent state
     * @return ESP_FAIL: failed to set safe state (partial failure)
     * @return Other: error codes propagated from the underlying IGpioHAL implementation
     *
     * @note Idempotent: can be called multiple times safely
     * @warning After deinit, the pin may float. Ensure external pull resistors if needed.
     */
    virtual esp_err_t deinit() = 0;

    // ========================================
    // Output Control
    // ========================================

    /**
     * @brief Turn the output ON (logical high)
     *
     * Sets the output to logical ON state. The physical GPIO level depends on the
     * inverted_logic setting from constructor:
     * - normal logic (false): physical HIGH = ON
     * - inverted logic (true): physical LOW = ON
     *
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_STATE: component not initialized
     * @return ESP_FAIL: hardware operation failed
     * @return Other: error codes propagated from the underlying IGpioHAL implementation
     *
     * @note Updates internal state is_on() to true on success
     */
    virtual esp_err_t turn_on() = 0;

    /**
     * @brief Turn the output OFF (logical low)
     *
     * Sets the output to logical OFF state. The physical GPIO level depends on the
     * inverted_logic setting from constructor:
     * - normal logic (false): physical LOW = OFF
     * - inverted logic (true): physical HIGH = OFF
     *
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_STATE: component not initialized
     * @return ESP_FAIL: hardware operation failed
     * @return Other: error codes propagated from the underlying IGpioHAL implementation
     *
     * @note Updates internal state is_on() to false on success
     */
    virtual esp_err_t turn_off() = 0;

    /**
     * @brief Toggle the current output state
     *
     * If currently ON, turns OFF. If currently OFF, turns ON.
     * Equivalent to: turn_on() if is_on() == false, else turn_off()
     *
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_STATE: component not initialized
     * @return ESP_FAIL: hardware operation failed
     * @return Other: error codes propagated from turn_on/turn_off
     *
     * @note Internal state is updated atomically with the hardware operation
     */
    virtual esp_err_t toggle() = 0;

    // ========================================
    // Advanced Configuration
    // ========================================

    /**
     * @brief Set GPIO drive capability (current strength)
     *
     * Configures the output drive strength of the GPIO pin. This affects the
     * maximum current the pin can source/sink.
     *
     * @param strength Drive strength level (GPIO_DRIVE_CAP_0 through GPIO_DRIVE_CAP_3)
     * @return ESP_OK on success
     * @return ESP_ERR_INVALID_STATE: component not initialized
     * @return ESP_ERR_INVALID_ARG: invalid drive strength value
     * @return ESP_FAIL: hardware operation failed
     * @return Other: error codes propagated from the underlying IGpioHAL implementation
     *
     * @note Higher drive strength may cause faster edge rates and more EMI
     * @warning Exceeding pin's maximum current can damage the hardware
     * @see driver/gpio.h for GPIO_DRIVE_CAP_* definitions
     */
    virtual esp_err_t set_drive_capability(gpio_drive_cap_t strength) = 0;

    // ========================================
    // Status & Information
    // ========================================

    /**
     * @brief Get current logical state
     *
     * Returns the last known logical state, independent of physical level.
     *
     * @return true Output is logically ON
     * @return false Output is logically OFF
     *
     * @note State is only updated after successful turn_on/turn_off/toggle operations
     */
    virtual bool is_on() const = 0;

    /**
     * @brief Check if component is initialized
     *
     * @return true Component is initialized and ready for operations
     * @return false Component is not initialized or has been deinitialized
     */
    virtual bool is_initialized() const = 0;

    /**
     * @brief Get the GPIO pin number
     *
     * @return gpio_num_t The pin number used by this instance
     *
     * @note Returns the pin configured at construction, valid even before init()
     */
    virtual gpio_num_t get_pin() const = 0;
};