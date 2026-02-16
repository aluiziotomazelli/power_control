# Host Testing with Google Test (GTest)

This directory contains host-based tests for the component. Host testing allows you to run unit tests on your development machine (Linux) instead of the target microcontroller, enabling faster development cycles and the use of advanced mocking frameworks like Google Mock.

## Google Test Integration

Instead of bundling the Google Test source code in this repository, we use CMake's **FetchContent** module. This approach keeps the repository lightweight and ensures we can easily manage dependencies.

### The GTest Wrapper Component

In ESP-IDF, every piece of code is treated as a component. To integrate GTest seamlessly, we use a "wrapper component" located in `host_test/gtest`.

This component's `CMakeLists.txt` is responsible for:
1. **Fetching GTest**: Downloading the specified version of Google Test/Mock from GitHub.
2. **ESP-IDF Integration**: Registering itself as an IDF component so that other components (like your tests) can simply use `REQUIRES gtest`.
3. **Handling the Build Flow**: Using guards like `CMAKE_BUILD_EARLY_EXPANSION` to ensure the download only happens during the actual build phase, preventing errors during IDF's component requirement discovery.

## How to Run Tests

### Prerequisites

- ESP-IDF environment set up and sourced.
- A Linux host machine (or WSL).
- The project root directory must be named `power_control` (or you must ensure the component name is correctly resolved by the build system).

### Execution Steps

1. Navigate to the test project directory:
   ```bash
   cd host_test/test_power_control
   ```

2. Set the target to Linux:
   ```bash
   idf.py --preview set-target linux
   ```

3. Build the test:
   ```bash
   idf.py build
   ```

4. Run the executable:
   ```bash
   ./build/test_power_control.elf
   ```

## Why FetchContent?

- **Clean Repo**: No need to store thousands of lines of external code.
- **Version Control**: Easy to upgrade GTest by changing a single URL/Tag.
- **Automation**: The library is automatically downloaded and built as part of the standard `idf.py build` flow.
