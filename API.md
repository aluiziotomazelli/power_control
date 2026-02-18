# Power Control API Reference

This document provides a detailed reference for the Power Control component API, including the public interface, implementation details, and supported types.

## Public Interface: `IPowerControl`

The `IPowerControl` interface defines the contract for controlling power outputs via GPIO pins. It abstracts away hardware details like active-high vs active-low configurations.

### Lifecycle Management

#### `init`
Initializes the power control hardware. Configures the GPIO pin as an output and applies the initial state.

**Returns:**
- `ESP_OK`: Initialization successful or already initialized.
- `ESP_ERR_INVALID_ARG`: GPIO number is invalid or cannot be used as output.
- `ESP_ERR_NO_MEM`: Memory allocation failed in HAL layer.
- `ESP_FAIL`: Hardware configuration failed.
- `Other`: Error codes propagated from the underlying HAL.

**Note:** The GPIO is forced to a known safe state during initialization.

#### `deinit`
Deinitializes the power control. Sets the GPIO to a safe state (low) and releases hardware resources.

**Returns:**
- `ESP_OK`: Deinitialization successful or already deinitialized.
- `ESP_FAIL`: Failed to set safe state (partial failure).
- `Other`: Error codes propagated from the underlying HAL.

**Warning:** After deinit, the pin may float. Ensure external pull resistors if needed.

---

### Output Control

#### `turn_on`
Sets the output to logical **ON** state.

**Returns:**
- `ESP_OK`: Success.
- `ESP_ERR_INVALID_STATE`: Component not initialized.
- `ESP_FAIL`: Hardware operation failed.

**Note:** The physical level (HIGH/LOW) depends on the `inverted_logic` configuration.

#### `turn_off`
Sets the output to logical **OFF** state.

**Returns:**
- `ESP_OK`: Success.
- `ESP_ERR_INVALID_STATE`: Component not initialized.
- `ESP_FAIL`: Hardware operation failed.

#### `toggle`
Toggles the current output state (ON to OFF, or OFF to ON).

**Returns:**
- `ESP_OK`: Success.
- `ESP_ERR_INVALID_STATE`: Component not initialized.
- `ESP_FAIL`: Hardware operation failed.

---

### Advanced Configuration

#### `set_drive_capability`
Configures the output drive strength (current capability) of the GPIO pin.

**Parameters:**
- `strength`: `gpio_drive_cap_t`
  - Drive strength level (0 to 3).

**Returns:**
- `ESP_OK`: Success.
- `ESP_ERR_INVALID_STATE`: Component not initialized.
- `ESP_ERR_INVALID_ARG`: Invalid drive strength value.

**Warning:** Exceeding pin's maximum current can damage the hardware.

---

### Status & Information

#### `is_on`
Returns the current logical state of the output.

**Returns:**
- `bool`: `true` if logically ON, `false` otherwise.

#### `is_initialized`
Checks if the component is initialized and ready for operations.

**Returns:**
- `bool`: `true` if initialized, `false` otherwise.

#### `get_pin`
Retrieves the GPIO pin number configured for this instance.

**Returns:**
- `gpio_num_t`: The configured GPIO pin number.

---

## Implementation: `PowerControl`

The `PowerControl` class is the concrete implementation of `IPowerControl`. It uses **Dependency Injection** to interact with the hardware via an `IGpioHAL` instance.

### Constructor

```cpp
PowerControl(IGpioHAL &hal, gpio_num_t gpio, bool inverted_logic = false, bool initial_on = false)
```

**Parameters:**
| Parameter | Type | Description |
| :--- | :--- | :--- |
| `hal` | `IGpioHAL &` | Reference to the GPIO HAL implementation. |
| `gpio` | `gpio_num_t` | GPIO pin number to control. |
| `inverted_logic`| `bool` | `true` for active LOW, `false` for active HIGH (default). |
| `initial_on` | `bool` | Initial logical state after `init()` is called (default: `false`). |

**Note on Dependency Injection:**
The `PowerControl` component does not create its own HAL. You must instantiate a concrete HAL (like `GpioHAL` for ESP-IDF) and pass it to the constructor. This allows for easier unit testing by injecting a Mock HAL.

---

## Types and Constants

### `gpio_drive_cap_t`
Defines the drive capability of the GPIO pin.

| Constant | Value | Description |
| :--- | :--- | :--- |
| `GPIO_DRIVE_CAP_0` | 0 | Weakest (~10 mA) |
| `GPIO_DRIVE_CAP_1` | 1 | Stronger |
| `GPIO_DRIVE_CAP_2` | 2 | Medium (default, ~20 mA) |
| `GPIO_DRIVE_CAP_3` | 3 | Strongest (~40 mA) |

**Note:** Actual current values depend on the specific ESP32 chip variant. Refer to the hardware technical reference manual.
