# Changelog

All notable changes to this project will be documented in this file.

## [1.0.2] - 2026-07-11

### Changed
- Migrated internal GPIO HAL implementation to centralized `idf_hals` external submodule.
- Removed local `gpio_hal.hpp` and `i_gpio_hal.hpp` files.
- Replaced local mock with centralized `MockGpioHAL` from `idf_hals`.
- Adapted project cmake configuration to require `idf_hals`.

## [1.0.1] - 2026-04-23

### Changed
- Wrapped all classes and interfaces in `power_control` namespace to avoid naming conflicts.
- `power_control.hpp` now internally includes `gpio_hal.hpp` for a cleaner application interface.

## [1.0.0] - 2025-02-17

### Added
- Initial release.
- Basic power control functionality.
- Support for normal and inverted logic.
- Drive capability configuration.
- Complete test suite with 100% line coverage.
