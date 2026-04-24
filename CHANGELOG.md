# Changelog

All notable changes to this project will be documented in this file.

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
