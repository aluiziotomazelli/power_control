# Power Control Component

A component for controlling power to external devices via GPIO, specifically designed for sensor applications in low-power systems.

## Table of Contents

- [Power Control Component](#power-control-component)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
  - [Typical Applications](#typical-applications)
    - [Sensor Power Cycling](#sensor-power-cycling)
    - [Ideal for:](#ideal-for)
  - [Architecture](#architecture)
  - [Operation Modes](#operation-modes)
    - [Direct Drive](#direct-drive)
    - [Transistor Drive](#transistor-drive)
      - [NPN Transistor (Low-Side Switching)](#npn-transistor-low-side-switching)
      - [PNP Transistor (High-Side Switching)](#pnp-transistor-high-side-switching)
  - [Current Capability](#current-capability)
  - [Usage Examples](#usage-examples)
    - [Direct Sensor Control](#direct-sensor-control)
    - [NPN Transistor Control](#npn-transistor-control)
    - [PNP Transistor Control](#pnp-transistor-control)
    - [Multiple Sensors](#multiple-sensors)
  - [API Reference](#api-reference)
  - [Integration Notes](#integration-notes)
    - [Dependency Injection](#dependency-injection)
    - [Current ESP-IDF Integration](#current-esp-idf-integration)
    - [Testing](#testing)
  - [License](#license)
  - [Author](#author)
  - [Changelog](#changelog)

## Overview

The Power Control component provides a clean, testable interface for controlling power to sensors and peripherals via GPIO pins. It's particularly useful in battery-powered applications where sensors need to be powered only during readings to save energy.

Key features:
- **Energy saving**: Power sensors only when needed
- **Flexible logic**: Support for normal and inverted logic
- **Drive capability**: Adjustable output current for different loads
- **Clean architecture**: Dependency injection for easy testing
- **Lifecycle management**: Proper init/deinit with safe states

## Typical Applications

### Sensor Power Cycling
```cpp
// In low-power applications, power the sensor only during readings
power.turn_on();      // Power up sensor
vTaskDelay(10);       // Warm-up time (sensor-specific)
sensor.read();        // Perform measurement
power.turn_off();     // Power down sensor to save energy
```

### Ideal for:
- Battery-powered IoT devices
- Environmental monitoring stations
- Wearable devices
- Any application using sleep modes

## Architecture

The component uses dependency injection for maximum testability and flexibility:

```text
┌──────────────────┐
│   Application    │
└────────┬─────────┘
         │
┌────────▼─────────┐
│  IPowerControl   │  Interface (public)
└────────┬─────────┘
         │
┌────────▼─────────┐
│   PowerControl   │  Implementation
└────────┬─────────┘
         │ uses
┌────────▼─────────┐
│     IGpioHAL     │  HAL Interface
└────────┬─────────┘
         │
┌────────▼─────────┐
│     GpioHAL      │  ESP-IDF implementation
└──────────────────┘
```

## Operation Modes

### Direct Drive

For low-current sensors that can be powered directly from a GPIO pin (typically up to 20-40 mA depending on configuration).

```cpp
// Direct drive configuration (normal logic, active HIGH)
PowerControl direct_power(hal, GPIO_NUM_4, false, false);
direct_power.init();
direct_power.turn_on();  // GPIO goes HIGH, sensor powers on
```

**Use when:**
- Sensor current < GPIO maximum (check datasheet)
- Sensor voltage matches GPIO level (usually 3.3V)
- Simple, minimal component count desired

### Transistor Drive

For higher current sensors or different voltage requirements, use external transistors.

#### NPN Transistor (Low-Side Switching)

NPN transistors are used as low-side switches. The transistor turns on when the base voltage is about 0.6V higher than the emitter (connected to ground). This is straightforward when driving from a digital output (3.3V or 5V).

**Circuit:**
- GPIO → Base resistor → NPN base
- NPN emitter → GND
- Load between Vcc and NPN collector

**Configuration:**
```cpp
// For NPN: HIGH = ON, LOW = OFF (normal logic)
PowerControl npn_power(hal, GPIO_NUM_4, false, false);
npn_power.turn_on();   // GPIO HIGH → transistor ON → load powered
npn_power.turn_off();  // GPIO LOW → transistor OFF → load off
```

#### PNP Transistor (High-Side Switching)

PNP transistors are used for high-side switching, where the load is connected to ground. The transistor turns on when the base voltage is about 0.6V lower than the emitter (connected to Vcc), meaning the base must be pulled LOW to turn it on.

**Circuit:**
- GPIO → Base resistor → PNP base
- PNP emitter → Vcc
- Load between PNP collector and GND

**Configuration:**
```cpp
// For PNP: LOW = ON, HIGH = OFF (inverted logic)
PowerControl pnp_power(hal, GPIO_NUM_4, true, false);  // inverted_logic = true
pnp_power.turn_on();   // GPIO LOW → transistor ON → load powered
pnp_power.turn_off();  // GPIO HIGH → transistor OFF → load off
```

## Current Capability

The component supports adjusting the GPIO drive strength to match different load requirements:

```cpp
typedef enum {
    GPIO_DRIVE_CAP_0       = 0,    // Weak (~10 mA)
    GPIO_DRIVE_CAP_1       = 1,    // Stronger
    GPIO_DRIVE_CAP_2       = 2,    // Medium (default, ~20 mA)
    GPIO_DRIVE_CAP_DEFAULT = 2,    // Default
    GPIO_DRIVE_CAP_3       = 3,    // Strongest (~40 mA)
} gpio_drive_cap_t;
```

**Usage:**
```cpp
power.init();
power.set_drive_capability(GPIO_DRIVE_CAP_3);  // Increase current capability
```

⚠️ **Important**: Always consult your ESP32 variant datasheet for exact current limits. Different chips (ESP32, ESP32-S3, ESP32-C3) may have different capabilities.

## Usage Examples

### Direct Sensor Control

```cpp
#include "power_control.hpp"
#include "gpio_hal.hpp"

void app_main() {
    // Create HAL and PowerControl instances
    GpioHAL hal;
    PowerControl sensor_power(hal, GPIO_NUM_4, false, false);
    
    // Initialize
    if (sensor_power.init() != ESP_OK) {
        ESP_LOGE("APP", "Failed to initialize power control");
        return;
    }
    
    // Power cycle sensor for each reading
    while (1) {
        sensor_power.turn_on();           // Power up sensor
        vTaskDelay(pdMS_TO_TICKS(10));    // Warm-up time
        
        float reading = read_sensor();    // Your sensor reading function
        
        sensor_power.turn_off();           // Power down to save energy
        
        printf("Sensor reading: %.2f\n", reading);
        vTaskDelay(pdMS_TO_TICKS(5000));   // Wait 5 seconds before next read
    }
}
```

### NPN Transistor Control

```cpp
// For driving higher current loads via NPN transistor
GpioHAL hal;
PowerControl load_power(hal, GPIO_NUM_5, false, false);  // normal logic

load_power.init();
load_power.set_drive_capability(GPIO_DRIVE_CAP_2);  // Adjust as needed

// Control load
load_power.turn_on();   // GPIO HIGH → transistor ON
// Load is powered
load_power.turn_off();  // GPIO LOW → transistor OFF
```

### PNP Transistor Control

```cpp
// For high-side switching with PNP transistor
GpioHAL hal;
PowerControl load_power(hal, GPIO_NUM_5, true, false);  // inverted logic

load_power.init();

// Control load (inverted logic: ON = GPIO LOW)
load_power.turn_on();   // GPIO LOW → transistor ON
// Load is powered
load_power.turn_off();  // GPIO HIGH → transistor OFF
```

### Multiple Sensors

```cpp
GpioHAL hal;

// Different sensors with different configurations
PowerControl sensor1(hal, GPIO_NUM_4, false, false);  // Direct drive
PowerControl sensor2(hal, GPIO_NUM_5, true, false);   // PNP transistor
PowerControl sensor3(hal, GPIO_NUM_6, false, false);  // NPN transistor

// Initialize all
sensor1.init();
sensor2.init();
sensor3.init();

// Power up sensors sequentially
sensor1.turn_on();
vTaskDelay(pdMS_TO_TICKS(5));
sensor2.turn_on();
vTaskDelay(pdMS_TO_TICKS(5));
sensor3.turn_on();

// Read all sensors
read_all_sensors();

// Power down (order doesn't matter)
sensor3.turn_off();
sensor2.turn_off();
sensor1.turn_off();
```

## API Reference

For a detailed description of the component's interface and implementation details, see [API.md](API.md).

## Integration Notes

### Dependency Injection
The component requires a `GpioHAL` instance to be injected. This design enables:
- **Unit testing**: Use `MockGpioHAL` in tests
- **Flexibility**: Easy to port to different platforms
- **Clear separation**: Hardware details isolated in HAL

### Current ESP-IDF Integration
The component is designed to work with ESP-IDF v4.4 and above. It uses:
- `driver/gpio.h` for GPIO operations
- `esp_log.h` for logging
- FreeRTOS for delays (in examples)

### Testing
The component comes with comprehensive unit tests using Google Test framework. Tests achieve:
- 100% line coverage
- 93.8% branch coverage
- Zero memory leaks (validated with Valgrind)

## License

This component is licensed under the MIT License - see the LICENSE file for details.

## Author

[github.com/aluiziotomazelli]


## Changelog

For a history of changes to this project, see [CHANGELOG.md](CHANGELOG.md).